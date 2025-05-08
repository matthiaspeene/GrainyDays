// GrainSpawner.cpp – implementation -------------------------------------------
#include "GrainSpawner.h"

void GrainSpawner::prepare(double sampleRate, int maxBlockSize)
{
	this->sampleRate = sampleRate;
	this->maxBlockSize = maxBlockSize;
	// 1)  Set up the grain scheduler // TBA:: Use DOD for this?
	for (auto& v : voices)
	{
		v.active = false;                // all voices are off
		v.midiNote = -1;                // no MIDI note assigned
		v.cursor = 0.0;                 // no grains scheduled yet
		v.interval = 0.0;               // no interval set
	}
}

void GrainSpawner::setParameterBank(const ParameterBank* params) noexcept
{
    this->params = params;
}

// ────────────────────────────────────────────────────────────────
// Public entry point – called once per audio block
void GrainSpawner::processMidi(const juce::MidiBuffer& midi,
    GrainPool& pool)
{
    currentSampleOffset = 0;                // start at the first frame

    // Walk MIDI events in ascending sample order
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        const int  samplePos = metadata.samplePosition;

        // 2-A · First, schedule grains up to this event
        advanceTime(samplePos - currentSampleOffset, pool);
        currentSampleOffset = samplePos;

        // 2-B · Then handle the MIDI itself
        if (msg.isNoteOn())      handleNoteOn(msg.getNoteNumber());
        else if (msg.isNoteOff()) handleNoteOff(msg.getNoteNumber());
    }

    // 2-C · Finish the tail of the block
    advanceTime(maxBlockSize - currentSampleOffset, pool);
}

void GrainSpawner::setSample(const LoadedSample* source)
{
	sample = source; 
}

// ────────────────────────────────────────────────────────────────
// Move time forward and drop grains for every active voice
void GrainSpawner::advanceTime(int numSamples, GrainPool& pool)
{
    if (numSamples <= 0) return;

    for (auto& v : voices)
    {
        if (!v.active) continue;

        double cursor = v.cursor;           // position of next grain

        while (cursor < numSamples)
        {
            const int delay = static_cast<int>(cursor);

            // Pick a free slot (drop if pool is full)
            const int index = findFreeGrainIndex(pool);
            if (index >= 0)
                spawnGrain(index, pool, delay);   // sample-accurate start
            // else { /* overflow → graceful drop */ }

            cursor += v.interval;           // schedule the following grain
        }

        v.cursor = cursor - numSamples;     // spill-over into next block
    }
}

// ────────────────────────────────────────────────────────────────
// MIDI helpers – start/stop one VoiceSpawner
void GrainSpawner::handleNoteOn(int note)
{
    auto& v = voices[note];
    v.active = true;
    v.midiNote = note;
    v.interval = sampleRate / params->grainDensity->load(std::memory_order_relaxed);
    v.cursor = 0.0;                    // first grain hits immediately
}

void GrainSpawner::handleNoteOff(int note)
{
    voices[note].active = false;
}

int GrainSpawner::findFreeGrainIndex(const GrainPool& pool) const
{
	// TBA::Can this be optimised?
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
    pool.delay[index] = delayOffset;

    const float seconds = params->grainLength->load(std::memory_order_relaxed);
    pool.frames[index] = static_cast<int>(seconds * sampleRate + 0.5f); // round to nearest

	const float dbGain = params->grainVolume->load(std::memory_order_relaxed);
	pool.gain[index] = juce::Decibels::decibelsToGain(dbGain); // convert to linear gain
    pool.pitch[index] = params->grainPitch->load(std::memory_order_relaxed);
	pool.pan[index] = params->grainPan->load(std::memory_order_relaxed);
	pool.samplePos[index] = 0;          // start at the beginning of the sample //TBA:: add sample offset

	DBG("Spawned grain " << index << " at " << delayOffset
		<< " samples, length " << pool.frames[index] << " frames");
}


