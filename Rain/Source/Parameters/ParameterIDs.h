// ─── ParameterIDs.h ──────────────────────────────────────────────────────────────
#pragma once
#include <array>
#include <cstddef>

namespace ParamID
{
    enum class ID : std::size_t
    {
        playMode,
        grainRate,
        delayRandomRange,
        midiRootNote,
        grainPitchMin,
        grainPitchMax,
        grainVolumeMin,
        grainVolumeMax,
        grainPanMin,
        grainPanMax,
        grainPositionMin,
        grainPositionMax,
        grainEnvAttack,
        grainEnvSustainLength,
        grainEnvRelease,
        grainEnvAttackCurve,
        grainEnvReleaseCurve,
		voiceAttack,
		voiceDecay,
		voiceSustain,
		voiceRelease,
		voiceAttackPower,
		voiceDecayPower,
		voiceReleasePower,
        Count        // ← compile-time size
    };

    inline constexpr std::array<const char*, static_cast<std::size_t>(ID::Count)> Names = {
        "playMode",
        "grainRate",
        "delayRandomRange",
        "midiRootNote",
        "grainPitchMin",
        "grainPitchMax",
        "grainVolumeMin",
        "grainVolumeMax",
        "grainPanMin",
        "grainPanMax",
        "grainPositionMin",
        "grainPositionMax",
        "grainEnvAttack",
        "grainEnvSustainLength",
        "grainEnvRelease",
        "grainEnvAttackCurve",
        "grainEnvReleaseCurve",
		"voiceAttack",
		"voiceDecay",
		"voiceSustain",
		"voiceRelease",
		"voiceAttackPower",
		"voiceDecayPower",
		"voiceReleasePower"
    };

    static_assert(Names.size() == static_cast<std::size_t>(ID::Count),
        "Names table out of sync with enum");

    /// Array-index helper
    [[nodiscard]] constexpr std::size_t idx(ID id) noexcept
    {
        return static_cast<std::size_t> (id);
    }

    /// C-string name for JUCE’s ParameterID
    [[nodiscard]] constexpr const char* toChars(ID id) noexcept
    {
        return Names[idx(id)];
    }
}
