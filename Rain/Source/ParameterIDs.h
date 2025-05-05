#pragma once
// All parameter IDs in one namespace = typo-free code.
namespace ParamID
{
    // ───────── top-level
    inline constexpr const char* masterGain = "masterGain";
    inline constexpr const char* grainDensity = "grainDensity";

    // ───────── Grain group
    inline constexpr const char* grainPitch = "grainPitch";
    inline constexpr const char* grainVolume = "grainVolume";

    // ───────── Filter group
    inline constexpr const char* filterCutoff = "filterCutoff";
    inline constexpr const char* filterResonance = "filterResonance";

    // ───────── non-exposed (internal / thread-safe)
    inline constexpr const char* currentGrainIndex = "currentGrainIndex";
    inline constexpr const char* randomSeed = "randomSeed";
}
