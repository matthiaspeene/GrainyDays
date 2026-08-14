#pragma once
#include <atomic>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include "../DSP/GrainPool.h"

struct GrainVisualData
{
	static constexpr std::size_t kMaxGrains = GrainPool::kMaxGrains;

	void clear() noexcept
	{
		for (auto& grainIsActive : active)
			grainIsActive.store(false, std::memory_order_relaxed);

		totalSamplesRendered.store(0, std::memory_order_relaxed);
	}

	alignas(64) std::atomic<uint64_t> totalSamplesRendered { 0 };
	alignas(64) std::atomic<bool> active[kMaxGrains] = {};

	alignas(64) uint64_t startTime[kMaxGrains]; // Number of samples at the start of the grain
	alignas(64) int length[kMaxGrains]; // samples

	alignas(64) double samplePos[kMaxGrains]; // initial sample offset in source
	alignas(64) int sampleLength[kMaxGrains]; // source length for this grain

	alignas(64) int envAttackTime[kMaxGrains]; // samples
	alignas(64) int envReleaseTime[kMaxGrains]; // samples
	alignas(64) float envAttackCurve[kMaxGrains]; // Power
	alignas(64) float envReleaseCurve[kMaxGrains]; // Power

	alignas(64) float maxGain[kMaxGrains];
	alignas(64) float step[kMaxGrains];     // step size in samples
};
