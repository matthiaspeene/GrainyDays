#pragma once

#include "GrainPool.h"
#include "GrainSpawner.h"
#include "GrainProcessor.h"
#include "ParameterBank.h"
#include "LoadedSample.h"

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

private:

    const ParameterBank* params = nullptr;

    double sampleRate = 44100.0;
    int maxBlockSize = 512;

    GrainPool pool;
    GrainSpawner spawner;
    GrainProcessor processor;
};
