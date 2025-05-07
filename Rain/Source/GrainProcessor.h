#pragma once
#include "GrainPool.h"
#include <juce_audio_basics/juce_audio_basics.h>

class GrainProcessor
{
public:
    // Light enough to inline too
    inline void prepare(double sr, int maxBlock) noexcept
    {
        sampleRate = sr;
        maxBlockSize = maxBlock;
    }

    // Hot path – body is in .inl
    inline void process(GrainPool& pool, juce::AudioBuffer<float>& output) noexcept;

private:
    double sampleRate = 44100.0;
    int    maxBlockSize = 512;
};

// Pull inline bodies into every TU that includes this header.
#include "GrainProcessor.inl"
