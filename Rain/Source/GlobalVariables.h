#pragma once
#include <atomic>

inline std::atomic<uint64_t> gTotalSamplesRendered = 0;
inline int gSampleSize = 0; // Sample size in samples