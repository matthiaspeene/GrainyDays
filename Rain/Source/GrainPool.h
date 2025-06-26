#pragma once
#include <bitset>
#include <cstddef>
#include <array>

struct GrainPool
{
    static constexpr std::size_t kMaxGrains = 4096;

    std::bitset<kMaxGrains> active;                  // 1 = slot busy

    alignas(64) int     delay[kMaxGrains];        // in samples
    alignas(64) int     frames[kMaxGrains];        // remaining frames
    alignas(64) double  samplePos[kMaxGrains];       // readhead
    alignas(64) float   step[kMaxGrains];
    alignas(64) float   gain[kMaxGrains];        // 0 … 1
    alignas(64) float   pan[kMaxGrains];        // –1 … +1
    alignas(64) int     length[kMaxGrains];
    alignas(64) int     envAttackFrames[kMaxGrains];
    alignas(64) int     envReleaseFrames[kMaxGrains];
    alignas(64) float   envAttackCurve[kMaxGrains];
    alignas(64) float   envReleaseCurve[kMaxGrains];
	alignas(64) uint8_t voiceIdx[kMaxGrains]; // which voice/midi note is playing this grain

    void clear() { active.reset(); }
};
