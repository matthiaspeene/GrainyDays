#pragma once
#include <bitset>
#include <cstddef>
#include <array>
#include "GrainPool.h"

struct GrainVisualData
{
	static constexpr std::size_t kMaxGrains = GrainPool::kMaxGrains;
	
	alignas (64) std::atomic<bool> active[kMaxGrains] = {};



	alignas(64) uint64_t startTime[kMaxGrains]; // Number of samples at the start of the grain
	alignas(64) int length[kMaxGrains]; // samples

	alignas(64) int samplePos[kMaxGrains];    // sample offset in source

	alignas(64) int envAttackTime[kMaxGrains]; // samples
	alignas(64) int envReleaseTime[kMaxGrains]; // samples
	alignas(64) float envAttackCurve[kMaxGrains]; // Power
	alignas(64) float envReleaseCurve[kMaxGrains]; // Power

	alignas(64) float maxGain[kMaxGrains];
	alignas(64) float step[kMaxGrains];     // step size in samples
};

inline GrainVisualData gGrainVisualData; // Global instance for visualization data