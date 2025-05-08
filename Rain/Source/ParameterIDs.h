#pragma once
// All parameter IDs in one namespace = typo-free code.
namespace ParamID
{
    // ───────── top-level
    inline constexpr const char* masterGain = "masterGain";
    inline constexpr const char* grainDensity = "grainDensity";

	// ───────── Voice group
	inline constexpr const char* midiRootNote = "midiRootNote";

    // ───────── Grain group
    inline constexpr const char* grainPitch = "grainPitch";
    inline constexpr const char* grainVolume = "grainVolume";
	inline constexpr const char* grainPan = "grainPan";
	inline constexpr const char* grainPosition = "grainPosition";

    // ───────── Filter group
    inline constexpr const char* filterCutoff = "filterCutoff";
    inline constexpr const char* filterResonance = "filterResonance";

	// ───────── Env group
	inline constexpr const char* envAttack = "envAttack";
	inline constexpr const char* envSustainLength = "envSustainLength";
	inline constexpr const char* envRelease = "envRelease";
	inline constexpr const char* envAttackCurve = "envAttackCurve";
	inline constexpr const char* envReleaseCurve = "envReleaseCurve";

    // ───────── non-exposed (internal / thread-safe)
    inline constexpr const char* currentGrainIndex = "currentGrainIndex";
    inline constexpr const char* randomSeed = "randomSeed";
}
