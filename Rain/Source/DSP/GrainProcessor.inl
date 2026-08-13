/*==============================================================================
   GrainProcessor.inl  – inline definitions
==============================================================================*/
#pragma once
#include "GrainProcessor.h"
#include "VoiceEnvelope.h"

#include <algorithm>   // std::fill_n
#include <cmath>       // std::pow, std::floor

/*──────────────────────────────────────────────────────────────────────────────
  prepare – allocate per-voice scratch buses once at start-up
──────────────────────────────────────────────────────────────────────────────*/
inline void GrainProcessor::prepare(double sr, int maxBlock) noexcept
{
    sampleRate = sr;

    busStride = maxBlock;                                    // frames / channel
    const std::size_t total =
        static_cast<std::size_t>(VoicePool::kMaxVoices) * 2 * busStride;

    voiceBus.assign(total, 0.0f);                            // allocate & zero

#if JUCE_DEBUG
    DBG("voiceBus alloc: "
        << (total * sizeof(float)) / 1024 << " kB   stride = " << busStride);
#endif
}

/*──────────────────────────────────────────────────────────────────────────────
  process – render one audio block
──────────────────────────────────────────────────────────────────────────────*/
inline void GrainProcessor::process(GrainPool& pool,
    VoicePool& voices,
    juce::AudioBuffer<float>& output) noexcept
{
    output.clear();

    const int nOutFrames = output.getNumSamples();
    const int nOutCh = output.getNumChannels();

    /* ───────── grow bus if host delivers a larger buffer ────────────── */
    if (voiceBus.empty() || nOutFrames > busStride)
    {
        busStride = nOutFrames;                                // grow, never shrink
        const std::size_t total =
            static_cast<std::size_t>(VoicePool::kMaxVoices) * 2 * busStride;

        voiceBus.assign(total, 0.0f);                        // re-alloc & zero
    }

    jassert(nOutFrames <= busStride);

    /* ───────── resolve sample source ─────────────────────────────────── */
    const auto* srcBuf = sampleSource.buffer.get();
    if (srcBuf == nullptr)
        return;                                               // no sample loaded

    const int nSrcCh = srcBuf->getNumChannels();
    const int nSrcFrames = srcBuf->getNumSamples();

    /* ───────── clear the slice we touch this callback ────────────────── */
    std::fill_n(voiceBus.data(),
        static_cast<std::size_t>(VoicePool::kMaxVoices) * 2 * nOutFrames,
        0.0f);

    /* ───────── helper: grain ASR envelope ────────────────────────────── */
    const auto grainEnv = [&](std::size_t gi, int framesLeft) -> float
        {
            const int total = pool.length[gi];
            const int atk = pool.envAttackFrames[gi];
            const int rel = pool.envReleaseFrames[gi];

            if (total <= 0 || atk + rel >= total)
                return 0.0f;

            if (framesLeft > total - atk)                      // attack
            {
                const float t = float(total - framesLeft) / atk;
                return std::pow(t, pool.envAttackCurve[gi]);
            }
            if (framesLeft < rel)                              // release
            {
                const float t = float(rel - framesLeft) / rel;
                return std::pow(1.0f - t, pool.envReleaseCurve[gi]);
            }
            return 1.0f;                                       // sustain
        };

    /*──────────────────────────────────────────────────────────────────────
      PASS 1 – grains → voice buses
    ──────────────────────────────────────────────────────────────────────*/
    for (std::size_t g = 0; g < GrainPool::kMaxGrains; ++g)
    {
        if (!pool.active[g])
            continue;

        /* guard: valid voice index -------------------------------------- */
        const int voiceId = pool.voiceIdx[g];
        if (voiceId < 0 || voiceId >= VoicePool::kMaxVoices)
        {
            DBG("*** BAD voiceId " << voiceId << "  in grain " << g);
            pool.active.reset(g);
            continue;
        }

        /* A. handle start delay ----------------------------------------- */
        int delay = pool.delay[g];
        if (delay >= nOutFrames)
        {
            pool.delay[g] = delay - nOutFrames;
            continue;
        }

        const int startFrame = std::max(0, delay);
        const int wantFrames = std::min(pool.frames[g],
            nOutFrames - startFrame);

        double      readPos = pool.samplePos[g];
        const double step = pool.step[g];
        const int    maxSrc = int(std::floor((nSrcFrames - 1 - readPos) /
            step) + 1.0);
        const int    framesHere = std::min(wantFrames, maxSrc);

        /* guard: frame count -------------------------------------------- */
        if (framesHere <= 0 || startFrame + framesHere > nOutFrames)
        {
            DBG("*** BAD framesHere (" << framesHere << ") in grain " << g);
            pool.active.reset(g);
            continue;
        }

        /* B. static grain gain + pan ------------------------------------ */
        const float baseGain = pool.gain[g];
        const float pan = pool.pan[g];
        const auto  gChGain = [=](int ch) -> float
            {
                if (nOutCh == 1)
                    return baseGain;
                return (ch == 0)
                    ? baseGain * (1.0f - pan) * 0.5f
                    : baseGain * (1.0f + pan) * 0.5f;
            };

        /* C. inner sample loop ------------------------------------------ */
        for (int ch = 0; ch < nOutCh; ++ch)
        {
            const int    srcCh = std::min(ch, nSrcCh - 1);
            const float* src = srcBuf->getReadPointer(srcCh);
            float* dst = busPtr(voiceId, ch) + startFrame;

            /* guard: pointer range -------------------------------------- */
            const std::size_t offs =
                static_cast<std::size_t>((voiceId * 2 + ch) * busStride + startFrame);
            if (offs + framesHere > voiceBus.size())
            {
                DBG("*** BUS overrun risk in grain " << g
                    << "  ch=" << ch << "  offs=" << offs
                    << "  frames=" << framesHere
                    << "  total=" << voiceBus.size());
                pool.active.reset(g);
                break;
            }

            double      rp = readPos;
            int         fL = pool.frames[g];
            const float gCh = gChGain(ch);

            for (int s = 0; s < framesHere; ++s, --fL)
            {
                const float env = grainEnv(g, fL);
                const int   idx = int(rp);
                const float frac = float(rp - idx);
                const float samp = src[idx] + frac * (src[idx + 1] - src[idx]);

                dst[s] += gCh * env * samp;
                rp += step;
            }
        }

        /* D. bookkeeping ------------------------------------------------ */
        pool.samplePos[g] += step * framesHere;
        pool.frames[g] -= framesHere;
        pool.delay[g] = 0;
        if (pool.frames[g] <= 0 || pool.samplePos[g] >= nSrcFrames - 1)
            pool.active.reset(g);
    }

    /*──────────────────────────────────────────────────────────────────────
      PASS 2 – update voice ADSR once per sample, mix buses to output
    ──────────────────────────────────────────────────────────────────────*/
    float* outL = output.getWritePointer(0);
    float* outR = (nOutCh > 1) ? output.getWritePointer(1) : nullptr;

    for (int s = 0; s < nOutFrames; ++s)
    {
        voice::env::updateOneSample(voices);             // all voices, once

        float mixL = 0.0f, mixR = 0.0f;

        for (std::size_t v = 0; v < VoicePool::kMaxVoices; ++v)
        {
            if (!voices.active.test(v))
                continue;

            const float vGain = voices.level[v];
            mixL += vGain * busPtr(v, 0)[s];
            if (outR)
                mixR += vGain * busPtr(v, 1)[s];
        }

        outL[s] += mixL;
        if (outR)
            outR[s] += mixR;
    }
}
