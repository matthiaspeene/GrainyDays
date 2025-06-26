#include "GrainEngine.h"


GrainEngine::GrainEngine()
	: spawner(voices)
{
	// Initialize the grain pool and voice pool
	pool.clear();
	voices.clear();
};

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
	if (processor.getSample().buffer == nullptr)
	{
		output.clear();
		return; // No sample loaded
	}

	spawner.processMidi(midi, pool);
    processor.process(pool, voices, output);
}

void GrainEngine::setLoadedSample(const LoadedSample& sample)
{
	processor.setSampleSource(sample); // Source is stored in the proccesor for quick acces
    spawner.setSample(&processor.getSample());  
}

