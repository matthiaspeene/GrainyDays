// GrainSpawner.cpp – implementation -------------------------------------------
#include "GrainSpawner.h"

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

// ────────────────────────────────────────────────────────────────
// Public entry point – called once per audio block
void GrainSpawner::processMidi(const juce::MidiBuffer& midi,
    GrainPool& pool, float grainsPerSecond)
{
    currentSampleOffset = 0;                // start at the first frame
	spawnedGrains = false;                  // reset grain spawn flag

	snapShot = loadSampleSnapShot(); // take a snapshot of the current parameters

    // Walk MIDI events in ascending sample order
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        const int  samplePos = metadata.samplePosition;

        // 2-A · First, schedule grains up to this event
        advanceTime(samplePos - currentSampleOffset, pool, grainsPerSecond);
        currentSampleOffset = samplePos;

        // 2-B · Then handle the MIDI itself
        if (msg.isNoteOn())      handleNoteOn(msg.getNoteNumber());
        else if (msg.isNoteOff()) handleNoteOff(msg.getNoteNumber());
    }

    // 2-C · Finish the tail of the block
    advanceTime(maxBlockSize - currentSampleOffset, pool, grainsPerSecond);
}

void GrainSpawner::setSample(const LoadedSample* source)
{
	sample = source; 
}

// ────────────────────────────────────────────────────────────────
// Move time forward and drop grains for every active voice
void GrainSpawner::advanceTime(int numSamples, GrainPool& pool, float grainsPerSecond)
{
    if (numSamples <= 0) return;

    if (grainsPerSecond < 1) // don't procces new grains at low gyro
        return;

    float samplesPerGrain = sampleRate / grainsPerSecond;

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

			cursor += samplesPerGrain;   // next grain in this voice
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

    pool.frames[index] = totalHostFrames;
    pool.grainLength[index] = totalHostFrames;

    initializeGainPan(pool, index);
    initializeStepSize(pool, index, midiNote);
    initializeEnvelope(pool, index, hostRate);
    initializePosition(pool, index);
    initializeDelay(pool, index, delayOffset, hostRate);
    
	// So instead of marking the bitset and copying all parameters to the UI at the end like this:
	spawnedGrains = true;
	spawnedGrainIndexes.set(index); // Mark this grain as spawned

    // We simply call:
	copyGrainToUI(index, pool); // Copy grain parameters to UI
}

// Helper function implementations:
void GrainSpawner::initializeGainPan(GrainPool& pool, int index)
{
    pool.gain[index] = juce::Decibels::decibelsToGain(snapShot.dbGain + rng.nextFloat() * snapShot.gainRand);
    pool.pan[index] = snapShot.panVal + (rng.nextFloat() * snapShot.panRand);
}

void GrainSpawner::initializeStepSize(GrainPool& pool, int index, int midiNote)
{
    double step = sample->sampleRate / sampleRate;

    if (snapShot.rootMidi >= 0 && snapShot.rootMidi < 128 && snapShot.rootMidi != midiNote)
        step *= std::pow(2.0, (midiNote - snapShot.rootMidi) / 12.0);

    float pitch = snapShot.pitchSemi + (rng.nextFloat() * snapShot.pitchRand);
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
    float pos = snapShot.posVal + (rng.nextFloat() * snapShot.posRand);
    pool.samplePos[index] = static_cast<int>((sample->buffer->getNumSamples() * pos) / 100.0f);
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

}
