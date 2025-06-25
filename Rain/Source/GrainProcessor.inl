/*==============================================================================
 GrainProcessor::process  – called once per audio block
==============================================================================*/
void GrainProcessor::process(GrainPool& pool,
    juce::AudioBuffer<float>& output) noexcept
{
    TRACE_DSP();
    output.clear();

    const int nOutFrames = output.getNumSamples();
    const int nOutCh = output.getNumChannels();

    /*------------------------------------------------------------------*/
    /* 1. Resolve the source buffer                                     */
    /*------------------------------------------------------------------*/
    const auto* srcBuf = sampleSource.buffer.get();
    jassert(srcBuf != nullptr);

    const int nSrcCh = srcBuf->getNumChannels();
    const int nSrcFrames = srcBuf->getNumSamples();          // last valid = nSrcFrames-1

    /*------------------------------------------------------------------*/
    /* 2. Helper: envelope value from “frames-left”                      */
    /*------------------------------------------------------------------*/
    const auto envGain = [&](const GrainPool& p, std::size_t gi, int framesLeft) -> float
        {
            const int total = p.length[gi];
            const int atk = p.envAttackFrames[gi];
            const int rel = p.envReleaseFrames[gi];

            /* Sanity guard – malformed grains                              */
            if (total <= 0 || atk + rel >= total)
                return 0.0f;

            /* Attack phase                                                 */
            if (framesLeft > total - atk)
            {
                const float t = float(total - framesLeft) / atk;           // 0 … 1
                return std::pow(t, p.envAttackCurve[gi]);
            }

            /* Release phase                                                */
            if (framesLeft < rel)
            {
                const float t = float(rel - framesLeft) / rel;             // 0 … 1
                return std::pow(1.0f - t, p.envReleaseCurve[gi]);
            }

            /* Sustain phase                                                */
            return 1.0f;
        };

    /*------------------------------------------------------------------*/
    /* 3. Walk the grain pool                                           */
    /*------------------------------------------------------------------*/
    for (std::size_t g = 0; g < GrainPool::kMaxGrains; ++g)
    {
        if (!pool.active[g])
            continue;

        /* 3-A  Handle start delay                                      */
        int delay = pool.delay[g];
        if (delay >= nOutFrames)
        {
            pool.delay[g] = delay - nOutFrames;   // keep waiting
            continue;
        }

        /* 3-B  Determine how many frames this grain can emit            */
        const int startFrame = juce::jmax(0, delay);
        const int wantFrames = juce::jmin(pool.frames[g], nOutFrames - startFrame);

        double readHead = pool.samplePos[g];   // *floating* play-head
        const double step = pool.step[g];

        const int maxSrcFrames = static_cast<int> (
            std::floor((nSrcFrames - 1 - readHead) / step) + 1.0);

        const int framesHere = juce::jmin(wantFrames, maxSrcFrames);
        if (framesHere <= 0)
        {
            pool.active.reset(g);
            continue;
        }

        /* 3-C  Pre-compute static gain & pan                            */
        const float gain = pool.gain[g];
        const float pan = pool.pan[g];

        const auto chGain = [gain, pan](int ch, int totalCh) -> float
            {
                if (totalCh == 1)   return gain;

                return (ch == 0)
                    ? gain * (1.0f - pan) * 0.5f    // left
                    : gain * (1.0f + pan) * 0.5f;   // right
            };

        /*----------------------------------------------------------------*/
        /* 3-D  Inner mix loop (linear interp + envelope)                  */
        /*----------------------------------------------------------------*/
        for (int ch = 0; ch < nOutCh; ++ch)
        {
            const int   srcCh = juce::jmin(ch, nSrcCh - 1);
            const float gCh = chGain(ch, nOutCh);

            const float* src = srcBuf->getReadPointer(srcCh);
            float* dst = output.getWritePointer(ch) + startFrame;

            double rp = readHead;          // channel-local copy
            int    fL = pool.frames[g];    // frames left (before current sample)

            for (int s = 0; s < framesHere; ++s, --fL)
            {
                /* Envelope ----------------------------------------------*/
                const float env = envGain(pool, g, fL);

                /* Linear interpolation ----------------------------------*/
                const int   idx = static_cast<int>(rp);
                const float frac = static_cast<float>(rp - idx);
                const float samp = src[idx] + frac * (src[idx + 1] - src[idx]);

                dst[s] += gCh * env * samp;
                rp += step;
            }
        }

        /* 3-E  Book-keeping                                             */
        readHead += step * framesHere;
        pool.samplePos[g] = readHead;
        pool.frames[g] -= framesHere;
        pool.delay[g] = 0;

        if (pool.frames[g] <= 0 || readHead >= nSrcFrames - 1)
            pool.active.reset(g);
    }
}
