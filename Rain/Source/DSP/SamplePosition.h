#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace samplePosition
{
inline double fromPercent(int numSamples, float percent) noexcept
{
    if (numSamples <= 1)
        return 0.0;

    const auto normalised = std::clamp(percent, 0.0f, 100.0f) / 100.0f;
    return static_cast<double>(numSamples - 1) * normalised;
}

inline int availableOutputFrames(int numSamples, double readPosition, double step) noexcept
{
    if (numSamples <= 1 || step <= 0.0 || !std::isfinite(readPosition) || !std::isfinite(step))
        return 0;

    // Linear interpolation reads both idx and idx + 1, so readPosition must
    // remain strictly below the final sample index.
    const auto remainingSourceSamples = static_cast<double>(numSamples - 1) - readPosition;
    if (remainingSourceSamples <= 0.0)
        return 0;

    const auto frames = std::ceil(remainingSourceSamples / step);
    return static_cast<int>(std::min(frames, static_cast<double>(std::numeric_limits<int>::max())));
}
}
