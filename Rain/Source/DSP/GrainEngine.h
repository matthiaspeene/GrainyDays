#pragma once

#include "GrainPool.h"
#include "VoicePool.h"
#include "GrainSpawner.h"
#include "GrainProcessor.h"
#include "../Parameters/ParameterBank.h"
#include "../Extras/LoadedSample.h"

#include <juce_audio_basics/juce_audio_basics.h>

class GrainEngine
{
public:
    GrainEngine();

    void setParameterBank(const ParameterBank* bank) noexcept;

    void prepare(double sampleRate, int maxBlockSize);
    void reset();
    void process(juce::AudioBuffer<float>& output, const juce::MidiBuffer& midi);

    void setLoadedSample(const LoadedSample& sample);
    GrainVisualData& getGrainVisualData() noexcept { return visualData; }

private:

    const ParameterBank* params = nullptr;

    double sampleRate = 44100.0;
    int maxBlockSize = 512;

    GrainPool pool;
	VoicePool voices;
    GrainVisualData visualData;
    GrainSpawner spawner;
    GrainProcessor processor;
};
