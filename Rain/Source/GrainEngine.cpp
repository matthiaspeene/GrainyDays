#include "GrainEngine.h"


GrainEngine::GrainEngine() = default;

void GrainEngine::setParameterBank(const ParameterBank* bank) noexcept
{
    params = bank;           // store for Engine-level use
    spawner.setParameterBank(bank);   // hand to sub-modules that need it
    // processor usually doesn't need the live bank; it gets snapshots per grain
}

void GrainEngine::prepare(double sr, int blockSize)
{
    sampleRate = sr;
    maxBlockSize = blockSize;

    spawner.prepare(sr, blockSize);
    processor.prepare(sr, blockSize);
    pool.clear();
}

void GrainEngine::reset()
{
    pool.clear();
}

void GrainEngine::process(juce::AudioBuffer<float>& output, const juce::MidiBuffer& midi)
{
    spawner.processMidi(midi, pool);
    processor.process(pool, output);
}

void GrainEngine::setLoadedSample(const LoadedSample& sample)
{
	processor.setSampleSource(sample); // Source is stored in the proccesor for quick acces
    spawner.setSample(&processor.getSample());  
}

