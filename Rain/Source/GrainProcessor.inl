#pragma once
#include <juce_core/juce_core.h>   // for juce::Random

// ───────────────────────────────────────────────────────────────────────────
inline void GrainProcessor::processOneGrain(int i,
    const GrainPool& pool,
    juce::AudioBuffer<float>& output)
{
    const int numCh = output.getNumChannels();
    const int numSamp = output.getNumSamples();

    const int start = pool.delay[i];
    const int len = pool.length[i];
    const float gain = pool.gain[i];
    const float pitch = pool.pitch[i];  // not used yet

    if (start >= numSamp || len <= 0)
        return;

    const int writeCount = juce::jmin(len, numSamp - start);

    for (int ch = 0; ch < numCh; ++ch)
    {
        float* dst = output.getWritePointer(ch);

        for (int s = 0; s < writeCount; ++s)
        {
            const int idx = start + s;

            // temporary white-noise grain
            float sample = gain * (juce::Random::getSystemRandom().nextFloat() * 2.f - 1.f);

            dst[idx] += sample;
        }
    }
}

// ───────────────────────────────────────────────────────────────────────────
inline void GrainProcessor::process(const GrainPool& pool,
    juce::AudioBuffer<float>& output)
{
    output.clear();

    for (int i = 0; i < static_cast<int>(GrainPool::kMaxGrains); ++i)
        if (pool.active[i])
            processOneGrain(i, pool, output);
}
