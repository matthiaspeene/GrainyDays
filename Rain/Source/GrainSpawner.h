#pragma once

#include "GrainPool.h"
#include "ParameterManager.h"
#include "ParameterBank.h"

#include <juce_audio_basics/juce_audio_basics.h>

class GrainSpawner
{
public:
    void setParameterBank(const ParameterBank* bank) noexcept { params = bank; }

    void prepare(double sampleRate, int maxBlockSize);
    void processMidi(const juce::MidiBuffer& midi, GrainPool& pool);

private:
    const ParameterBank* params = nullptr;
    
    double sampleRate = 44100.0;
    int maxBlockSize = 512;

    int currentSampleOffset = 0;

    int findFreeGrainIndex(const GrainPool& pool) const;
    void spawnGrain(int index, GrainPool& pool, int delayOffset);
};
