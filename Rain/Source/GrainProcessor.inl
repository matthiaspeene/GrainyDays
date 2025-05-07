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
void GrainProcessor::process(GrainPool& pool, juce::AudioBuffer<float>& output) noexcept
{
    output.clear();
    const int numCh = output.getNumChannels();
    const int nSamps = output.getNumSamples();
    FastLCG rng; // Fast white noise for testing

    for (int i = 0; i < static_cast<int>(GrainPool::kMaxGrains); ++i)
    {
        if (!pool.active[i])
            continue;

        int d = pool.delay[i];
        if (d >= nSamps)
        {
            pool.delay[i] = d - nSamps;
            continue;
        }

        const int start = juce::jmax(0, d);
        const int framesHere = juce::jmin(pool.frames[i], nSamps - start);
        const float g = pool.gain[i];

        for (int ch = 0; ch < numCh; ++ch)
        {
            float* dst = output.getWritePointer(ch) + start;
            for (int s = 0; s < framesHere; ++s) // TBA:: replace with audio read
                dst[s] += g * rng.nextFloat();
        }

        pool.delay[i] = 0;
        pool.frames[i] -= framesHere;

        if (pool.frames[i] <= 0)
            pool.active.reset(i);
    }
}

