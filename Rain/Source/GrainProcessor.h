#pragma once
#include "GrainPool.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include "LoadedSample.h"
#include "VoicePool.h"
#include "VoiceEnvelope.h"

class GrainProcessor
{
public:
    // Light enough to inline too
    inline void prepare(double sr, int maxBlock) noexcept;

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
    inline void process(GrainPool& pool, VoicePool& voices, juce::AudioBuffer<float>& output) noexcept;

private:
    inline float* busPtr(std::size_t voice, int ch) noexcept
    {
        return voiceBus.data() + ((voice * 2 + ch) * busStride);
    }

    inline std::size_t totalBusSamples() const noexcept
    {
        return static_cast<std::size_t>(VoicePool::kMaxVoices) * 2 * busStride;
    }

    double sampleRate = 44100.0;
    int    maxBlockSize = 512;

	LoadedSample sampleSource;

    std::vector<float> voiceBus;
    int                busStride = 0;
};

// Pull inline bodies into every TU that includes this header.
#include "GrainProcessor.inl"
