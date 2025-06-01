#include "ParameterCreator.h"
#include "ParameterIDs.h"

using namespace juce;
using namespace ParamID;

// ──────────────────────────────────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────────────────────────────────
static NormalisableRange<float> linRange(float a, float b,
    float step = 0.01f,
    float skew = 1.0f)
{
    return { a, b, step, skew };
}

static NormalisableRange<float> logHzRange(float lo, float hi)
{
    return { lo, hi, 0.0f, std::log10f(hi / lo) };
}

// ──────────────────────────────────────────────────────────────────────────
// 1)  Build the APVTS layout
// ──────────────────────────────────────────────────────────────────────────
AudioProcessorValueTreeState::ParameterLayout ParameterCreator::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;

	/* Disabled
	layout.add(std::make_unique<AudioParameterFloat>(
		ParameterID{ gyroStrength, 1 }, "Gyro Strength",
		linRange(0.f, 200.f, 0.01f), 0.5f, " *"));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ masterGain, 1 }, "Master Gain",
        linRange(-60.f, 0.f, 0.1f), 0.f, " dB"));
	*/


	layout.add(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainDensity, 1 }, "Minimum Density",
		linRange(1.f, 6000.f, 1.f, 0.5f), 50.f, " grains/s"));

	layout.add(std::make_unique<AudioParameterFloat>(
		ParameterID{ delayRandomRange, 1 }, "Delay Random Range",
		linRange(0.f, 1.f, 0.01f), 0.0f, " s"));


	// Drop down menu
	layout.add(std::make_unique<AudioParameterChoice>(
		ParameterID{ playMode, 1 }, "Play Mode",
		StringArray{ "Midi", "Gyro", "Rotation" }, 0));

	// Number box, no slider
	layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ midiRootNote, 1 },
		"Root Note", linRange(0.f, 127.f, 1.f), 60.f, " MIDI note"));


    // ─── Grain group ─────────────────────────────────────────────────────
    auto grainGroup = std::make_unique<AudioProcessorParameterGroup>(
        "grainGroup", "Grain", "|");

    grainGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ grainPitch, 1 }, "Pitch",
        linRange(-24.f, 24.f, 0.1f), 0.0f, " st"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainPitchRandomRange, 1 }, "Pitch Random Range",
		linRange(0.f, 24.f, 0.1f), 0.0f, " st"));

    grainGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ grainVolume, 1 }, "Volume",
        linRange(-48.f, 0.f, 0.1f), 0.0f, " dB"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainVolumeRandomRange, 1 }, "Volume Random Range",
		linRange(-48.f, 0.f, 0.1f), 0.0f, " dB"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainPan, 1 }, "Pan",
		linRange(-1.f, 1.f, 0.01f), 0.0f, " %"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainPanRandomRange, 1 }, "Pan Random Range",
		linRange(-1.f, 1.f, 0.01f), 0.0f, " %"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainPosition, 1 }, "Position",
		linRange(0.f, 100.f, 0.01f), 0.0f, " %"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ grainPositionRandomRange, 1 }, "Position Random Range",
		linRange(0.f, 100.f, 0.01f), 0.0f, " %"));



    layout.add(std::move(grainGroup));

	// ─── Env group ─────────────────────────────────────────────────────
	auto envGroup = std::make_unique<AudioProcessorParameterGroup>(
		"envGroup", "Envelope", "|");

	envGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ envAttack, 1 }, "Attack",
		linRange(0.01f, 1.f, 0.01f), 0.01f, " s"));

    envGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ envSustainLength, 1 }, "Sustain Length",
        linRange(0.01f, 1.f, 0.01f), 0.01f, " s"
    ));

	envGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ envRelease, 1 }, "Release",
		linRange(0.01f, 1.f, 0.01f), 0.01f, " s"));

	envGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ envAttackCurve, 1 }, "Attack Curve",
		linRange(0.1f, 10.f, 0.01f), 1.0f));

	envGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ envReleaseCurve, 1 }, "Release Curve",
		linRange(0.1f, 10.f, 0.01f), 1.0f));

	layout.add(std::move(envGroup));

	auto modGroup = std::make_unique<AudioProcessorParameterGroup>(
		"modGroup", "Modulation", "|");
	// ─── Modulation group ────────────────────────────────────────────────
	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ velocityModGrainDensity, 1 }, "Velocity Mod Density",
		linRange(-200.f, 200.f), 0.f, " grains/s"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ velocityModGrainPitch, 1 }, "Velocity Mod Pitch",
		linRange(-24.f, 24.f, 0.1f), 0.0f, " st"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ velocityModGrainVolume, 1 }, "Velocity Mod Volume",
		linRange(-48.f, 64.f, 0.1f), 0.0f, " dB"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ velocityModGrainPan, 1 }, "Velocity Mod Pan",
		linRange(-1.f, 1.f, 0.01f), 0.0f, " %"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ velocityModGrainPosition, 1 }, "Velocity Mod Position",
		linRange(-100.f, 100.f, 0.01f), 0.0f, " %"));

	// Rotation
	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ rotZModGrainDensity, 1 }, "RotZ Mod Density",
		linRange(-200.f, 200.f), 0.f, " grains/s"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ rotZModGrainPitch, 1 }, "RotZ Mod Pitch",
		linRange(-24.f, 24.f, 0.1f), 0.0f, " st"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ rotZModGrainVolume, 1 }, "RotZ Mod Volume",
		linRange(-48.f, 64.f, 0.1f), 0.0f, " dB"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ rotZModGrainPan, 1 }, "RotZ Mod Pan",
		linRange(-1.f, 1.f, 0.01f), 0.0f, " %"));

	modGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ rotZModGrainPosition, 1 }, "RotZ Mod Position",
		linRange(-100.f, 100.f, 0.01f), 0.0f, " %"));

	layout.add(std::move(modGroup));

	/* Disabled
    // ─── Filter group ────────────────────────────────────────────────────
    auto filterGroup = std::make_unique<AudioProcessorParameterGroup>(
        "filterGroup", "Filter", "|");

    filterGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ filterCutoff, 1 }, "Cut-off",
        logHzRange(20.f, 20'000.f), 10'000.f, " Hz"));

    filterGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ filterResonance, 1 }, "Resonance",
        linRange(0.1f, 10.f, 0.01f, 0.5f), 1.0f));

    layout.add(std::move(filterGroup));
	*/

    return layout;                            // ← ready to pass to APVTS
}

// ──────────────────────────────────────────────────────────────────────────
// 2)  Non-exposed (internal) atomics
// ──────────────────────────────────────────────────────────────────────────
std::unordered_map<std::string, std::atomic<float>>
ParameterCreator::createNonExposed()
{
    using Map = std::unordered_map<std::string, std::atomic<float>>;
    Map map;
    map.emplace(currentGrainIndex, 0.0f);
    map.emplace(randomSeed, 0.0f);
    return map;
}

// ──────────────────────────────────────────────────────────────────────────
// 3)  Modulation atomics
// ──────────────────────────────────────────────────────────────────────────
std::unordered_map<juce::String, std::atomic<float>>
ParameterCreator::createModFloatMap()
{
    constexpr const char* modulatable[] = {
        masterGain, grainPitch, filterCutoff
    };

    std::unordered_map<juce::String, std::atomic<float>> map;
    for (auto* id : modulatable)
        map.emplace(juce::String(id) + "_mod", 0.0f);

    return map;
}
