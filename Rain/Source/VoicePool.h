#pragma once
#include <bitset>
#include <cmath>
#include <algorithm>

enum class Stage : uint8_t { Attack, Decay, Sustain, Release, Idle };

struct VoicePool
{
    static constexpr std::size_t kMaxVoices = 128;   // one per MIDI note
    std::bitset<kMaxVoices> active;

    /* dynamic state — changes every sample or block */
    alignas(64) Stage stage[kMaxVoices]{};
    alignas(64) float level[kMaxVoices]{};   // current sample value
    alignas(64) float levelStep[kMaxVoices]{};   // add this every sample
	alignas(64) int   stageSamplesLeft[kMaxVoices]{};

	alignas(64) float spawnCursor[kMaxVoices]{}; // next grain spawn time in samples

    /* static per-note parameters — written at note-on only */
	alignas(64) int   attackSamples[kMaxVoices]{};
    alignas(64) int   decaySamples[kMaxVoices]{};
    alignas(64) float sustainLevel[kMaxVoices]{};
    alignas(64) int   releaseSamples[kMaxVoices]{};
    alignas(64) float  releaseStart[kMaxVoices]{};   // initial level at note-off
    alignas(64) float attackPower[kMaxVoices]{};
    alignas(64) float decayPower[kMaxVoices]{};
	alignas(64) float releasePower[kMaxVoices]{};
    alignas(64) int   midiNote[kMaxVoices]{};            // 0-127, convenience

    void clear() { active.reset(); }
};
