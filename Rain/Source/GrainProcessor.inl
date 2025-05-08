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

    /*---------------------------------------------------------------------
        Resolve source buffer once – avoids repeated shared_ptr locking.
      --------------------------------------------------------------------*/
    const auto* srcBuf = sampleSource.buffer.get();
    jassert(srcBuf != nullptr);                       // make sure we loaded OK

    const int nSrcCh = srcBuf->getNumChannels();
    const int nSrcFrames = srcBuf->getNumSamples();

    /*---------------------------------------------------------------------
        Iterate the pool
      --------------------------------------------------------------------*/
    for (std::size_t i = 0; i < GrainPool::kMaxGrains; ++i)
    {
        if (!pool.active[i])
            continue;

        /*---------- countdown until this grain actually starts ---------*/
        int d = pool.delay[i];

        if (d >= nOutFrames)                  // nothing happens this block
        {
            pool.delay[i] = d - nOutFrames;
            continue;
        }

        /*---------- how many frames can we mix in this callback? -------*/
        const int startFrame = juce::jmax(0, d);
        const int framesWanted = juce::jmin(pool.frames[i],
            nOutFrames - startFrame);

        /* Make sure we do not run past the end of the sample.            */
        const int srcPos = pool.samplePos[i];
        const int framesAvail = nSrcFrames - srcPos;

        const int framesHere = juce::jmin(framesWanted, framesAvail);
        if (framesHere <= 0)
        {
            pool.active.reset(i);
            continue;
        }

        /*---------- pre-compute per-grain gain / pan -------------------*/
        const float g = pool.gain[i];
        const float pan = pool.pan[i];        // -1 … 0 … +1

        auto gainForCh = [g, pan](int ch, int totalOutCh) -> float
            {
                if (totalOutCh == 1)            return g;

                /* simple linear-pan: constant-power costs two sin/cos calls   *
                 * so keep the math trivial for now.                          */
                return (ch == 0) ? g * (1.0f - pan) * 0.5f
                    : g * (1.0f + pan) * 0.5f;
            };

        /*---------- mix -----------------------------------------------*/
        for (int ch = 0; ch < nOutCh; ++ch)
        {
            /* choose source channel – wrap if mono sample into stereo out */
            const int srcCh = juce::jmin(ch, nSrcCh - 1);

            const float* src = srcBuf->getReadPointer(srcCh) + srcPos;
            float* dst = output.getWritePointer(ch) + startFrame;

            const float gCh = gainForCh(ch, nOutCh);

            for (int s = 0; s < framesHere; ++s)
                dst[s] += gCh * src[s];
        }

        /*---------- book-keeping --------------------------------------*/
        pool.samplePos[i] += framesHere;
        pool.frames[i] -= framesHere;
        pool.delay[i] = 0;

        if (pool.frames[i] <= 0 || pool.samplePos[i] >= nSrcFrames)
            pool.active.reset(i);
    }
}

