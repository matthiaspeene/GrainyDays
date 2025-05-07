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
    alignas(64) float gain[kMaxGrains];   // constant per grain

	alignas(64) float pan[kMaxGrains];    // per-grain pan (UNUSED)
	alignas(64) float pitch[kMaxGrains];  // per-grain pitch (UNUSED)

    void clear()
    {
        active.reset();
    }
};
