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
    output.clear();

    const int nOutFrames = output.getNumSamples();
    const int nOutCh = output.getNumChannels();

    /*--------------------------------------------------------------*/
    /* 1.  Resolve source buffer once – no shared-ptr traffic        */
    /*--------------------------------------------------------------*/
    const auto* srcBuf = sampleSource.buffer.get();
    jassert(srcBuf != nullptr);

    const int nSrcCh = srcBuf->getNumChannels();
    const int nSrcFrames = srcBuf->getNumSamples();      // last valid index = nSrcFrames-1

    /*--------------------------------------------------------------*/
    /* 2.  Scan the grain pool                                      */
    /*--------------------------------------------------------------*/
    for (std::size_t i = 0; i < GrainPool::kMaxGrains; ++i)
    {
        if (!pool.active[i])
            continue;

        /*---- delay countdown ------------------------------------*/
        int d = pool.delay[i];
        if (d >= nOutFrames)
        {
            pool.delay[i] = d - nOutFrames;
            continue;
        }

        /*---- frames we’re *allowed* to write this call ----------*/
        const int startFrame = juce::jmax(0, d);
        const int framesWanted = juce::jmin(pool.frames[i],
            nOutFrames - startFrame);

        /*----------------------------------------------------------
          Bound by remaining source data *and* read step.
          We need the readPtr + (framesHere-1)*step < nSrcFrames-1
        ----------------------------------------------------------*/
        double  readPtr = pool.samplePos[i];      // fractional
        const double step = pool.step[i];      // already rate-corrected

        const int maxFromSrc = static_cast<int>(
            std::floor((nSrcFrames - 1 - readPtr) / step) + 1.0);

        const int framesHere = juce::jmin(framesWanted, maxFromSrc);
        if (framesHere <= 0)
        {
            pool.active.reset(i);
            continue;
        }

        /*---- pre-compute gain & pan -----------------------------*/
        const float g = pool.gain[i];
        const float pan = pool.pan[i];

        auto gainForCh = [g, pan](int ch, int totalOutCh) -> float
            {
                if (totalOutCh == 1)  return g;
                return (ch == 0) ? g * (1.0f - pan) * 0.5f
                    : g * (1.0f + pan) * 0.5f;
            };

        /*----------------------------------------------------------
          3.  Inner mix loop  (linear interpolation, per channel)
        ----------------------------------------------------------*/
        for (int ch = 0; ch < nOutCh; ++ch)
        {
            const int   srcCh = juce::jmin(ch, nSrcCh - 1);
            const float gCh = gainForCh(ch, nOutCh);

            const float* src = srcBuf->getReadPointer(srcCh);
            float* dst = output.getWritePointer(ch) + startFrame;

            double rp = readPtr;                // channel-local copy

            for (int s = 0; s < framesHere; ++s)
            {
                const int   idx = static_cast<int>(rp);
                const float frac = static_cast<float>(rp - idx);

                const float v0 = src[idx];
                const float v1 = src[idx + 1];           // safe: idx+1 < nSrcFrames
                const float samp = v0 + frac * (v1 - v0);  // 2-tap lerp

                dst[s] += gCh * samp;
                rp += step;
            }
        }

        /*---- book-keeping --------------------------------------*/
        readPtr += step * framesHere;   // same maths as loop
        pool.samplePos[i] = readPtr;
        pool.frames[i] -= framesHere;
        pool.delay[i] = 0;

        if (pool.frames[i] <= 0 || readPtr >= nSrcFrames - 1)
            pool.active.reset(i);
    }
}


