#include "GrainSpawner.h"

void GrainSpawner::prepare(double sr, int blockSize)
{
    sampleRate = sr;
    maxBlockSize = blockSize;
}

void GrainSpawner::processMidi(const juce::MidiBuffer& midi, GrainPool& pool)
{
    currentSampleOffset = 0;

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        const auto samplePos = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            const int grainIndex = findFreeGrainIndex(pool);
            if (grainIndex >= 0)
            {
                spawnGrain(grainIndex, pool, samplePos);
            }
        }
    }
}

int GrainSpawner::findFreeGrainIndex(const GrainPool& pool) const
{
    for (int i = 0; i < static_cast<int>(GrainPool::kMaxGrains); ++i)
    {
        if (!pool.active[i])
            return i;
    }

    return -1; // No free grain found
}

void GrainSpawner::spawnGrain(int index, GrainPool& pool, int delayOffset)
{
    pool.active.set(index);
    pool.gain[index] = params->grainVolume->load(std::memory_order_relaxed);
    pool.pitch[index] = params->grainPitch->load(std::memory_order_relaxed);
    pool.delay[index] = delayOffset;
    // lenght

    // TODO: Add more fields here as needed (envelope shape, pan, filter, etc.)
}
