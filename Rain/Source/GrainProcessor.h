#pragma once
#include "GrainPool.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include "LoadedSample.h"

class GrainProcessor
{
public:
    // Light enough to inline too
    inline void prepare(double sr, int maxBlock) noexcept
    {
        sampleRate = sr;
        maxBlockSize = maxBlock;
    }

    void setSampleSource(const LoadedSample& source)
    {
		DBG("GrainProcessor::setSampleSource: " << source.buffer->getNumSamples()
			<< " samples at " << source.sampleRate << " Hz");
        this->sampleSource = source;
    }

	const LoadedSample& getSample() const noexcept
	{
		return sampleSource;
	}

    // Hot path – body is in .inl
    inline void process(GrainPool& pool, juce::AudioBuffer<float>& output) noexcept;

private:
    double sampleRate = 44100.0;
    int    maxBlockSize = 512;

	LoadedSample sampleSource;
};

// Pull inline bodies into every TU that includes this header.
#include "GrainProcessor.inl"
