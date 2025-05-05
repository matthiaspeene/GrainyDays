#pragma once
#include <bitset>
#include <cstddef>
#include <array>

struct GrainPool
{
    static constexpr std::size_t kMaxGrains = 2048;

    std::array<float, kMaxGrains> gain{};
    std::array<float, kMaxGrains> pitch{};
    std::array<int, kMaxGrains> delay{};
    std::array<int, kMaxGrains> length{};
	std::array<int, kMaxGrains> position{};

    std::bitset<kMaxGrains> active;

    void clear()
    {
        active.reset();
    }
};
