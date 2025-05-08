#pragma once
#include <bitset>
#include <cstddef>
#include <array>

struct GrainPool
{
    static constexpr std::size_t kMaxGrains = 4096;

    std::bitset<kMaxGrains> active;   // 1 = slot busy
    alignas(64) int   delay[kMaxGrains];   // sample-accurate countdown
    alignas(64) int   frames[kMaxGrains];   // frames left to render
	alignas(64) int   grainLength[kMaxGrains]; // total length in samples

	alignas(64) int   samplePos[kMaxGrains];    // sample offset in source
	alignas(64) float step[kMaxGrains];     // step size for pitch and sampleratio diffrences

    alignas(64) float gain[kMaxGrains];
    alignas(64) float pan[kMaxGrains];
	//alignas(64) float pitch[kMaxGrains];  Made redundant by stepsize for now

    // Envelope
	alignas(64) int envAttackFrames[kMaxGrains];
	alignas(64) int envReleaseFrames[kMaxGrains];

	alignas(64) float envAttackCurve[kMaxGrains]; // Power
	alignas(64) float envReleaseCurve[kMaxGrains]; // Power

    void clear()
    {
        active.reset();
    }
};
