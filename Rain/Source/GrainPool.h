#pragma once
#include <bitset>
#include <cstddef>
#include <array>

struct GrainPool
{
    static constexpr std::size_t kMaxGrains = 4096;

    std::bitset<kMaxGrains> active;						// 1 = slot busy

    alignas(64) int   delay[kMaxGrains];				// Delay in samples
	alignas(64) int   frames[kMaxGrains];				// Remaining samples in the grain (playHead)
	alignas(64) int   samplePos[kMaxGrains];			// Sample offset in source
	alignas(64) float step[kMaxGrains];					// Sample step size in samples
	alignas(64) float gain[kMaxGrains];					// Gain in 0-1 range
	alignas(64) float pan[kMaxGrains];					// Pan in -1 to 1 range
	alignas(64) int   length[kMaxGrains];				// Total length in samples
	alignas(64) int   envAttackFrames[kMaxGrains];		// Attack length in samples
	alignas(64) int   envReleaseFrames[kMaxGrains];		// Release length in samples
	alignas(64) float envAttackCurve[kMaxGrains];		// Power
	alignas(64) float envReleaseCurve[kMaxGrains];		// Power

    void clear()
    {
        active.reset();
    }
};

