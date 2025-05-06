#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include "ParameterBank.h"
#include "GrainPool.h"

/*───────────────────────────────────────────────────────────────────────────*/
class GrainSpawner
{
public:
    // Lifecycle --------------------------------------------------------------
    void prepare(double sampleRate, int maxBlockSize);
    void setParameterBank(const ParameterBank* params) noexcept;

    void processMidi(const juce::MidiBuffer& midi, GrainPool& pool);

private:
    /* Helper ---------------------------------------------------------------*/
    struct VoiceGrainScheduler
    {
        bool   active = false;
        int    midiNote = -1;
        double cursor = 0.0;   // sample offset to the next grain
        double interval = 0.0;   // samples between grains for this note
    };
    static constexpr int kNumMidiNotes = 128;

    // Core helpers -----------------------------------------------------------
    void advanceTime(int numSamples, GrainPool& pool);
    void handleNoteOn(int midiNote);
    void handleNoteOff(int midiNote);

    int  findFreeGrainIndex(const GrainPool& pool) const;
    void spawnGrain(int index, GrainPool& pool, int delayOffset);

    /* State ----------------------------------------------------------------*/
    double        sampleRate = 44100.0;
    int           maxBlockSize = 0;
    int           currentSampleOffset = 0;

    VoiceGrainScheduler  voices[kNumMidiNotes]; // 4kb
    const ParameterBank* params = nullptr;
};
