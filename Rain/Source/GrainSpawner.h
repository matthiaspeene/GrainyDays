#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include "ParameterBank.h"
#include "GrainPool.h"
#include "LoadedSample.h"

/* Helpers ───────────────────────────────────────────────────────────────────────────*/
struct ParameterSnapshot {
    float dbGain, gainRand, gainMod;
    float panVal, panRand, panMod;
    float pitchSemi, pitchRand, pitchMod;
    float posVal, posRand, posMod;
    float envAttack, envRelease, envSustainLength;
    float envAttackCurve, envReleaseCurve;
    float delayRandomRange;
    int   rootMidi;
};

/*───────────────────────────────────────────────────────────────────────────*/
class GrainSpawner
{
public:
    // Lifecycle --------------------------------------------------------------
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

    bool playingRootNote = false;

	// UI helpers -------------------------------------------------------------
	void copyGrainToUI(int index, GrainPool& pool);

	juce::Random rng; // If other parts need this too move shared instance to the engine

    /* State ----------------------------------------------------------------*/
    double        sampleRate = 44100.0;
    int           maxBlockSize = 0;
    int           currentSampleOffset = 0;

    VoiceGrainScheduler  voices[kNumMidiNotes]; // 4kb

    const ParameterBank* params = nullptr;
    const LoadedSample* sample;

    //snapshot
	ParameterSnapshot snapShot;
};
