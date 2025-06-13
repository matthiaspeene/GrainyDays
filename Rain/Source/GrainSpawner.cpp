// GrainSpawner.cpp – implementation -------------------------------------------
#include "GrainSpawner.h"
#include "GrainVisualData.h"
#include "PluginProcessor.h"
#include "GlobalVariables.h"

void GrainSpawner::prepare(double sampleRate, int maxBlockSize)
{
	this->sampleRate = sampleRate;
	this->maxBlockSize = maxBlockSize;
	// 1)  Set up the grain scheduler // TBA:: Use DOD for this?
	for (auto& v : voices)
	{
		v.active = false;               // all voices are off
		v.midiNote = -1;                // no MIDI note assigned
		v.cursor = 0.0;                 // no grains scheduled yet
	}
}

void GrainSpawner::setParameterBank(const ParameterBank* params) noexcept
{
    this->params = params;
}

// Mode enum
enum class PlayMode
{
    Midi = 0
};

inline bool shouldPlayRoot(const ParameterBank& p, PlayMode mode) noexcept
{
	switch (mode) // Reimpliment if a mode like scrubbing is added.
    {
    default:                 
        return false;          // MIDI mode
    }
}

// ────────────────────────────────────────────────────────────────
// Entry point – called once per audio block
void GrainSpawner::processMidi(const juce::MidiBuffer& midi,
    GrainPool& pool)
{
    const PlayMode mode = static_cast<PlayMode>(params->playMode->load(std::memory_order_relaxed));
    const bool     root = (mode != PlayMode::Midi);
    const bool     gate = shouldPlayRoot(*params, mode);

    updateRootGate(gate);

    currentSampleOffset = 0;
	snapShot = loadSampleSnapShot(); // Take a snapshot of the current parameters for fast thread safe use

    // Walk MIDI events in ascending order
    for (const auto meta : midi)
    {
        const int pos = meta.samplePosition;
        advanceTime(pos - currentSampleOffset, pool);
        currentSampleOffset = pos;

        const auto msg = meta.getMessage();
        if (root)
        {
            if (msg.isNoteOff()) handleNoteOff(msg.getNoteNumber());
        }
        else
        {
            if (msg.isNoteOn())      handleNoteOn(msg.getNoteNumber());
            else if (msg.isNoteOff()) handleNoteOff(msg.getNoteNumber());
        }
    }

    // Finish the tail of the block
    advanceTime(maxBlockSize - currentSampleOffset, pool);
}

void GrainSpawner::updateRootGate(bool playRootNow)
{
    if (playRootNow == playingRootNote) return;           // nothing changed

	playingRootNote = playRootNow;                        // update state

    const int root = params->midiRootNote->load(std::memory_order_relaxed);
    if (playRootNow)
    {
		DBG("GrainSpawner: Starting root note " << root);
        handleNoteOn(root);
    }
    else
    {
		DBG("GrainSpawner: Stopping root note " << root);
        handleNoteOff(root);
    }
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

	const float grainsPerSec = sampleRate / (params->grainDensity->load(std::memory_order_relaxed));

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
                spawnGrain(index, pool, delay, v.midiNote);   // sample-accurate start
            // else { /* overflow → graceful drop */ }

			cursor += grainsPerSec;   // next grain in this voice
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

void GrainSpawner::spawnGrain(int index, GrainPool& pool, int delayOffset, int midiNote)
{
    pool.active.set(index);

    const double hostRate = sampleRate;
    const double lenSec = snapShot.envAttack + snapShot.envSustainLength + snapShot.envRelease;
    const int totalHostFrames = static_cast<int>(lenSec * hostRate + 0.5);

	DBG("GrainSpawner: Spawning grain at index " << index
		<< " with delay " << delayOffset
		<< ", total frames: " << totalHostFrames
		<< ", MIDI note: " << midiNote);

    pool.frames[index] = totalHostFrames;
    pool.length[index] = totalHostFrames;

    initializeGainPan(pool, index);
    initializeStepSize(pool, index, midiNote);
    initializeEnvelope(pool, index, hostRate);
    initializePosition(pool, index);
    initializeDelay(pool, index, delayOffset, hostRate);
    
	copyGrainToUI(index, pool);
}

// Helper function implementations:
void GrainSpawner::initializeGainPan(GrainPool& pool, int index)
{
    pool.gain[index] = juce::Decibels::decibelsToGain(snapShot.dbGain + rng.nextFloat() * snapShot.gainRand + snapShot.gainMod);
    pool.pan[index] = snapShot.panVal + (rng.nextFloat() * snapShot.panRand) + snapShot.panMod;
}

void GrainSpawner::initializeStepSize(GrainPool& pool, int index, int midiNote)
{
    double step = sample->sampleRate / sampleRate;

    if (snapShot.rootMidi >= 0 && snapShot.rootMidi < 128 && snapShot.rootMidi != midiNote)
        step *= std::pow(2.0, (midiNote - snapShot.rootMidi) / 12.0);

    float pitch = snapShot.pitchSemi + (rng.nextFloat() * snapShot.pitchRand) + snapShot.pitchMod;
    if (pitch != 0.0f)
        step *= std::pow(2.0, pitch / 12.0);

    pool.step[index] = static_cast<float>(step);
}

void GrainSpawner::initializeEnvelope(GrainPool& pool, int index, double hostRate)
{
    pool.envAttackFrames[index] = static_cast<int>(snapShot.envAttack * hostRate + 0.5);
    pool.envReleaseFrames[index] = static_cast<int>(snapShot.envRelease * hostRate + 0.5);
    pool.envAttackCurve[index] = snapShot.envAttackCurve;
    pool.envReleaseCurve[index] = snapShot.envReleaseCurve;
}

void GrainSpawner::initializePosition(GrainPool& pool, int index)
{
    float pos = snapShot.posVal + (rng.nextFloat() * snapShot.posRand) + snapShot.posMod;

	if (pos < 0.0f) pos = 0.0f; // Clamp to 0%
	if (pos > 100.0f) pos = 100.0f; // Clamp to 100%
    pool.samplePos[index] = static_cast<int>(sample->buffer->getNumSamples() * (pos/100));
}

void GrainSpawner::initializeDelay(GrainPool& pool, int index, int delayOffset, double hostRate)
{
    pool.delay[index] = delayOffset + static_cast<int>((rng.nextFloat() * snapShot.delayRandomRange) * hostRate + 0.5);
}


ParameterSnapshot GrainSpawner::loadSampleSnapShot()
{
    return ParameterSnapshot
    {
        .dbGain = params->grainVolume->load(),
        .gainRand = params->grainVolumeRandomRange->load(),
        .panVal = params->grainPan->load(),
        .panRand = params->grainPanRandomRange->load(),
        .pitchSemi = params->grainPitch->load(),
        .pitchRand = params->grainPitchRandomRange->load(),
		.posVal = params->grainPosition->load(),
		.posRand = params->grainPositionRandomRange->load(),
		.envAttack = params->envAttack->load(),
		.envRelease = params->envRelease->load(),
		.envSustainLength = params->envSustainLength->load(),
		.envAttackCurve = params->envAttackCurve->load(),
		.envReleaseCurve = params->envReleaseCurve->load(),
		.delayRandomRange = params->delayRandomRange->load(),
        .rootMidi = static_cast<int>(params->midiRootNote->load())
    };
}

void GrainSpawner::copyGrainToUI(int index, GrainPool& pool)
{
	if (gGrainVisualData.active[index].load(std::memory_order_relaxed))
	{
		DBG("GrainSpawner: Overwriting existing grain at index " << index);
	}

	gGrainVisualData.samplePos[index] = pool.samplePos[index];
    gGrainVisualData.startTime[index] = gTotalSamplesRendered.load(std::memory_order_relaxed) + static_cast<uint64_t>(pool.delay[index]);
    gGrainVisualData.length[index] = pool.length[index];
	gGrainVisualData.step[index] = pool.step[index];
	gGrainVisualData.envAttackTime[index] = pool.envAttackFrames[index];
	gGrainVisualData.envReleaseTime[index] = pool.envReleaseFrames[index];
	gGrainVisualData.envAttackCurve[index] = pool.envAttackCurve[index];
	gGrainVisualData.envReleaseCurve[index] = pool.envReleaseCurve[index];
	gGrainVisualData.maxGain[index] = pool.gain[index];
	gGrainVisualData.active[index].store(true, std::memory_order_relaxed); // Activate grain
}
