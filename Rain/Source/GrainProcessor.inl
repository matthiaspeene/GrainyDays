/* -----------------------------------------------------------------
   Fast LCG – ~3× cheaper than juce::Random in the inner loop
   -----------------------------------------------------------------*/
#include <bit>

struct FastLCG
{
    uint32_t state = 0x12345678u;
    inline float nextFloat() noexcept
    {
        state = state * 1664525u + 1013904223u;            // LCG
        return (state >> 8) * (1.0f / 0x00FFFFFFu) * 2.f - 1.f;
    }
};

/* -----------------------------------------------------------------
   GrainProcessor::process  – called once per audio block
   -----------------------------------------------------------------*/
void GrainProcessor::process(GrainPool& pool,
    juce::AudioBuffer<float>& output) noexcept
{
	TRACE_DSP();
    output.clear();

    const int nOutFrames = output.getNumSamples();
    const int nOutCh = output.getNumChannels();

    /*--------------------------------------------------------------*/
    /* 1.  Resolve source buffer once – no shared-ptr traffic        */
    /*--------------------------------------------------------------*/
    const auto* srcBuf = sampleSource.buffer.get();
    jassert(srcBuf != nullptr);

    const int nSrcCh = srcBuf->getNumChannels();
    const int nSrcFrames = srcBuf->getNumSamples();          // last valid index = nSrcFrames-1

    /* --- helper: envelope value from “frames left” ----------------*/
    auto envGain = [&](const GrainPool& p,
        std::size_t      gi,
        int              fLeft) -> float
        {
            const int totalN = p.length[gi];
            const int atkN = p.envAttackFrames[gi];
            const int relN = p.envReleaseFrames[gi];

            /* sanity guard ---------------------------------------------*/
            if (atkN + relN >= totalN || totalN <= 0)
                return 0.0f;

            /* ATTACK ----------------------------------------------------*/
            if (fLeft > totalN - atkN)
            {
                const float t = static_cast<float>(totalN - fLeft) / atkN;   // 0…1
                const float k = p.envAttackCurve[gi];
                return std::pow(t, k);
            }

            /* RELEASE ---------------------------------------------------*/
            if (fLeft < relN)
            {
                const float t = static_cast<float>(relN - fLeft) / relN;     // 0…1
                const float k = p.envReleaseCurve[gi];
                return std::pow(1.0f - t, k);
            }

            /* HOLD ------------------------------------------------------*/
            return 1.0f;
        };

	int activeGrains = 0;            // DBG
    /*--------------------------------------------------------------*/
    /* 2.  Scan the grain pool                                      */
    /*--------------------------------------------------------------*/
    for (std::size_t i = 0; i < GrainPool::kMaxGrains; ++i)
    {
        if (!pool.active[i])
            continue;

#if PERFETTO_ENABLED
        TRACE_EVENT_INSTANT("dsp", "grainProccesStart");
#endif

		activeGrains++; // DBG

        /* ---- delay countdown -----------------------------------*/
        int d = pool.delay[i];
        if (d >= nOutFrames)
        {
            pool.delay[i] = d - nOutFrames;
            continue;
        }

        /* ---- frames we’re allowed to write this block ----------*/
        const int startFrame = juce::jmax(0, d);
        const int framesWanted = juce::jmin(pool.frames[i],
            nOutFrames - startFrame);

        /* ---- clamp to end of sample ----------------------------*/
        double  readPtr = pool.samplePos[i];
        const double step = pool.step[i];

        const int maxFromSrc = static_cast<int>(
            std::floor((nSrcFrames - 1 - readPtr) / step) + 1.0);

        const int framesHere = juce::jmin(framesWanted, maxFromSrc);
        if (framesHere <= 0)
        {
            pool.active.reset(i);
            continue;
        }

        /* ---- pre-compute static gain & pan ---------------------*/
        const float g = pool.gain[i];
        const float pan = pool.pan[i];

        auto gainForCh = [g, pan](int ch, int totalOutCh) -> float
            {
                if (totalOutCh == 1)        return g;
                return (ch == 0) ? g * (1.0f - pan) * 0.5f
                    : g * (1.0f + pan) * 0.5f;
            };

        /*----------------------------------------------------------
          3.  Inner mix loop  (linear interp + envelope)
        ----------------------------------------------------------*/
        for (int ch = 0; ch < nOutCh; ++ch)
        {
            const int   srcCh = juce::jmin(ch, nSrcCh - 1);
            const float gCh = gainForCh(ch, nOutCh);

            const float* src = srcBuf->getReadPointer(srcCh);
            float* dst = output.getWritePointer(ch) + startFrame;

            double rp = readPtr;                            // channel-local copy
            int    fL = pool.frames[i];                     // frames left *before* sample

            for (int s = 0; s < framesHere; ++s, --fL)
            {
                /* envelope --------------------------------------*/
                const float e = envGain(pool, i, fL);       // 1 multiply worth

                /* sample fetch ----------------------------------*/
                const int   idx = static_cast<int>(rp);
                const float frac = static_cast<float>(rp - idx);
				const float samp = src[idx] + frac * (src[idx + 1] - src[idx]); // point of out of bounds

                dst[s] += gCh * e * samp;
                rp += step;
            }
        }

        /* ---- book-keeping -------------------------------------*/
        readPtr += step * framesHere;
        pool.samplePos[i] = readPtr;
        pool.frames[i] -= framesHere;
        pool.delay[i] = 0;

        if (pool.frames[i] <= 0 || readPtr >= nSrcFrames - 1)
            pool.active.reset(i);
    }

	//DBG("GrainProcessor::process: " << activeGrains << " active grains");
}