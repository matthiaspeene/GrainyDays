#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include "ParameterBank.h"
#include "GrainPool.h"
#include "VoicePool.h"
#include "LoadedSample.h"

/* Helpers ───────────────────────────────────────────────────────────────────────────*/
struct ParameterSnapshot {
    // Mods are not used at the moment, but reserved for future use
	float gainMin, gainMax, gainMod = 0.f;          // in dB
	float panMin, panMax, panMod = 0.f;             // -1 to 1
	float pitchMin, pitchMax, pitchMod = 0.f;       // in semitones
    float posMin, posMax, posMod = 0.f;
	float envAttack, envRelease, envSustainLength = 0.1f; // in seconds
    float envAttackCurve, envReleaseCurve = 1.f;
    float delayRandomRange = 0.f;
	int   rootMidi = -1; // -1 means no root note, otherwise 0-127
};

struct VoiceParameterSnapshot {
	float gain = 1.0f;
	float pan = 0.0f; // -1 to 1
	float envAttack, envDecay, envRelease = 1.f; // in seconds
	float envAttackCurve, envDecayCurve, envReleaseCurve = 1.f; // power
	float sustainLevel = 1.0f; // 0 to 1
};;

/*───────────────────────────────────────────────────────────────────────────*/
class GrainSpawner
{
public:
    explicit GrainSpawner(VoicePool& vp) : voices(vp) {}

    void prepare(double sampleRate, int maxBlockSize);
    void setParameterBank(const ParameterBank* params) noexcept;

    void processMidi(const juce::MidiBuffer& midi, GrainPool& pool);

    void setSample(const LoadedSample* source);

private:
    /* Helper ---------------------------------------------------------------*/
    struct VoiceGrainScheduler
    {
        bool   active = false;
        int    midiNote = -1;
        double cursor = 0.0;   // sample offset to the next grain
    };
    static constexpr int kNumMidiNotes = 128;

    // Core helpers -----------------------------------------------------------

    void updateRootGate(bool playRootNow);
    void advanceTime(int numSamples, GrainPool& pool);
    void handleNoteOn(int midiNote);
    void handleNoteOff(int midiNote);

    int  findFreeGrainIndex(const GrainPool& pool) const;
    void spawnGrain(int idx, GrainPool& pool, int delay, int midiNote);
    void initializeGainPan(GrainPool& pool, int index);
    void initializeStepSize(GrainPool& pool, int index, int midiNote);
    void initializeEnvelope(GrainPool& pool, int index, double hostRate);
    void initializePosition(GrainPool& pool, int index);
    void initializeDelay(GrainPool& pool, int index, int delayOffset, double hostRate);

    ParameterSnapshot loadSampleSnapShot();
	VoiceParameterSnapshot loadVoiceSnapShot();

    bool playingRootNote = false;

	// UI helpers -------------------------------------------------------------
	void copyGrainToUI(int index, GrainPool& pool);

	juce::Random rng; // If other parts need this too move shared instance to the engine

    /* State ----------------------------------------------------------------*/
    double        sampleRate = 44100.0;
    int           maxBlockSize = 0;
    int           currentSampleOffset = 0;

    VoicePool& voices;

    const ParameterBank* params = nullptr;
    const LoadedSample* sample = nullptr;

    //snapshot
	ParameterSnapshot snapShot;
	VoiceParameterSnapshot voiceSnapShot;
};
