// GrainSpawner.cpp – implementation -------------------------------------------
#include "GrainSpawner.h"
#include "GrainVisualData.h"
#include "PluginProcessor.h"
#include "GlobalVariables.h"
#include "VoiceEnvelope.h"
#include "ParameterIDs.h"

void GrainSpawner::prepare(double sampleRate, int maxBlockSize)
{
	this->sampleRate = sampleRate;
	this->maxBlockSize = maxBlockSize;

    voices.clear();
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
	TRACE_DSP();

    const PlayMode mode = static_cast<PlayMode>(params->get(ParamID::ID::playMode));
    const bool     root = (mode != PlayMode::Midi);
    const bool     gate = shouldPlayRoot(*params, mode);

    updateRootGate(gate);

    currentSampleOffset = 0;
	snapShot = loadSampleSnapShot(); // Take a snapshot of the current parameters for fast thread safe use
    voiceSnapShot = loadVoiceSnapShot();

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

    const int root = params->get(ParamID::ID::midiRootNote);
    if (playRootNow)
    {
		//DBG("GrainSpawner: Starting root note " << root);
        handleNoteOn(root);
    }
    else
    {
		//DBG("GrainSpawner: Stopping root note " << root);
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

	const float grainsPerSec = sampleRate / (params->get(ParamID::ID::grainRate));

    for (std::size_t v = 0; v < VoicePool::kMaxVoices; ++v)
    {
        if (!voices.active.test(v)) continue;

        double cursor = voices.spawnCursor[v];           // position of next grain

        while (cursor < numSamples)
        {
            const int delay = static_cast<int>(cursor);

            // Pick a free slot (drop if pool is full)
            const int index = findFreeGrainIndex(pool);
            if (index >= 0)
                spawnGrain(index, pool, delay, v);   // sample-accurate start
            // else { /* overflow → graceful drop */ }

			cursor += grainsPerSec;   // next grain in this voice
        }

        voices.spawnCursor[v] = cursor - numSamples;     // spill-over into next block
    }
}

// ────────────────────────────────────────────────────────────────
// MIDI helpers – start/stop one VoiceSpawner
void GrainSpawner::handleNoteOn(int note)
{
	voices.attackSamples[note] = static_cast<int>(voiceSnapShot.envAttack * sampleRate + 0.5);
	voices.decaySamples[note] = static_cast<int>(voiceSnapShot.envDecay * sampleRate + 0.5);
	voices.releaseSamples[note] = static_cast<int>(voiceSnapShot.envRelease * sampleRate + 0.5);
	voices.sustainLevel[note] = voiceSnapShot.sustainLevel;
	voices.attackPower[note] = voiceSnapShot.envAttackCurve;
	voices.decayPower[note] = voiceSnapShot.envDecayCurve;
	voices.releasePower[note] = voiceSnapShot.envReleaseCurve;

	voice::env::noteOn(voices, note); // Set voice active
}

void GrainSpawner::handleNoteOff(int note)
{
	voice::env::noteOff(voices, note); // Set voice inactive
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
    TRACE_DSP();
    pool.active.set(index);
    pool.voiceIdx[index] = static_cast<uint8_t>(midiNote);

    const double hostRate = sampleRate;
    const double lenSec = snapShot.envAttack + snapShot.envSustainLength + snapShot.envRelease;
    const int totalHostFrames = static_cast<int>(lenSec * hostRate + 0.5);

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
    pool.gain[index] = juce::Decibels::decibelsToGain(snapShot.gainMin + rng.nextFloat() * (snapShot.gainMax - snapShot.gainMin) + snapShot.gainMod);
    pool.pan[index] = snapShot.panMin + rng.nextFloat() * (snapShot.panMax - snapShot.panMin) + snapShot.panMod;
}

void GrainSpawner::initializeStepSize(GrainPool& pool, int index, int midiNote)
{
    double step = sample->sampleRate / sampleRate;

    if (snapShot.rootMidi >= 0 && snapShot.rootMidi < 128 && snapShot.rootMidi != midiNote)
        step *= std::pow(2.0, (midiNote - snapShot.rootMidi) / 12.0);

    float pitch = snapShot.pitchMin + rng.nextFloat() * (snapShot.pitchMax - snapShot.pitchMin) + snapShot.pitchMod;
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
    float pos = snapShot.posMin + rng.nextFloat() * (snapShot.posMax - snapShot.posMin) + snapShot.posMod;

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
        .gainMin = params->get(ParamID::ID::grainVolumeMin),
        .gainMax = params->get(ParamID::ID::grainVolumeMax),
        .panMin = params->get(ParamID::ID::grainPanMin),
        .panMax = params->get(ParamID::ID::grainPanMax),
        .pitchMin = params->get(ParamID::ID::grainPitchMin),
        .pitchMax = params->get(ParamID::ID::grainPitchMax),
		.posMin = params->get(ParamID::ID::grainPositionMin),
		.posMax = params->get(ParamID::ID::grainPositionMax),
		.envAttack = params->get(ParamID::ID::grainEnvAttack)/1000,
		.envRelease = params->get(ParamID::ID::grainEnvRelease)/1000,
		.envSustainLength = params->get(ParamID::ID::grainEnvSustainLength)/1000,
		.envAttackCurve = params->get(ParamID::ID::grainEnvAttackCurve),
		.envReleaseCurve = params->get(ParamID::ID::grainEnvReleaseCurve),
		.delayRandomRange = params->get(ParamID::ID::delayRandomRange)/1000,
        .rootMidi = static_cast<int>(params->get(ParamID::ID::midiRootNote))
    };
}

VoiceParameterSnapshot GrainSpawner::loadVoiceSnapShot()
{
    return VoiceParameterSnapshot
    {
        //.gain = juce::Decibels::decibelsToGain(params->get(ParamID::ID::voiceGain)), // TBA
        //.pan = params->get(ParamID::ID::voicePan),
        .envAttack = params->get(ParamID::ID::voiceAttack),
        .envDecay = params->get(ParamID::ID::voiceDecay),
        .envRelease = params->get(ParamID::ID::voiceRelease),
        .envAttackCurve = params->get(ParamID::ID::voiceAttackPower),
        .envDecayCurve = params->get(ParamID::ID::voiceDecayPower),
        .envReleaseCurve = params->get(ParamID::ID::voiceReleasePower),
        .sustainLevel = params->get(ParamID::ID::voiceSustain)
    };
}

void GrainSpawner::copyGrainToUI(int index, GrainPool& pool)
{
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
