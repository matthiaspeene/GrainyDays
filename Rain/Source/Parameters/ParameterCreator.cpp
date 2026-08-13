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

	layout.add(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainRate), 1}, "Grain Rate", // Would use ID::grainDensity which would be an int to string perhaps? If that's the case perhaps it would be more optimised to just use ID::grainDensity directly. If I want to use the string what dous it look like again?
		linRange(1.f, 6000.f, 0.1f, 0.5f), 50.f, " hz"));

	layout.add(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::delayRandomRange), 1}, "Delay Random Range", // This is not correct. Can we add a helper class in ParameterIDs.h to convert ID to string?
		linRange(0.f, 1000.f, 0.1f, 0.5f), 0.f, " ms"));


	// Drop down menu
	layout.add(std::make_unique<AudioParameterChoice>(
		ParameterID{ toChars(ID::playMode), 1 }, "Play Mode",
		StringArray{ "Midi", "Gyro", "Rotation" }, 0));

	// Number box, no slider
	layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ toChars(ID::midiRootNote), 1 },
		"Root Note", linRange(0.f, 127.f, 1.f), 60.f, " MIDI note"));


    // ─── Grain group ─────────────────────────────────────────────────────
    auto grainGroup = std::make_unique<AudioProcessorParameterGroup>(
        "grainGroup", "Grain", "|");

    grainGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ toChars(ID::grainPitchMin), 1 }, "Grain Pitch Min",
        linRange(-24.f, 24.f, 0.01f), 0.0f, " st"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainPitchMax), 1 }, "Grain Pitch Max",
		linRange(-24.f, 24.f, 0.01f), 0.0f, " st"));

    grainGroup->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ toChars(ID::grainVolumeMin), 1 }, "Grain Volume Min",
        linRange(-48.f, 16.f, 0.01f), 0.0f, " dB"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainVolumeMax), 1 }, "Grain Volume Max",
		linRange(-48.f, 16.f, 0.01f), 0.0f, " dB"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainPanMin), 1 }, "Grain Pan Min",
		linRange(-1.f, 1.f, 0.1f), 0.0f, ""));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainPanMax), 1 }, "Grain Pan Max",
		linRange(-1.f, 1.f, 0.1f), 0.0f, ""));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainPositionMin), 1 }, "Position Min",
		linRange(0.f, 100.f, 0.01f), 0.0f, " %"));

	grainGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainPositionMax), 1 }, "Position Max",
		linRange(0.f, 100.f, 0.01f), 0.0f, " %"));


	layout.add(std::move(grainGroup));

	// ─── Env group ─────────────────────────────────────────────────────
	auto grainShape = std::make_unique<AudioProcessorParameterGroup>(
		"grainShape", "GrainShape", "|");

	grainShape->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainEnvAttack), 1 }, "Attack",
		linRange(0.1f, 1000.f, 0.1f, 0.5f), 45.0f, " ms"));

	grainShape->addChild(std::make_unique<AudioParameterFloat>(
        ParameterID{ toChars(ID::grainEnvSustainLength), 1 }, "Sustain Length",
        linRange(0.1f, 1000.f, 0.1f, 0.5f), 0.1f, " ms"
    ));

	grainShape->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainEnvRelease), 1 }, "Release",
		linRange(0.1f, 1000.f, 0.1f, 0.5f), 45.0f, " ms"));

	grainShape->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainEnvAttackCurve), 1 }, "Attack Curve",
		linRange(0.1f, 10.f, 0.01f, 0.25f), 2.0f));

	grainShape->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::grainEnvReleaseCurve), 1 }, "Release Curve",
		linRange(0.1f, 10.f, 0.01f, 0.25f), 3.0f));

	layout.add(std::move(grainShape));

	// ─── Voice group ─────────────────────────────────────────────────────
	auto voiceEnvGroup = std::make_unique<AudioProcessorParameterGroup>(
		"voiceEnvGroup", "Voice Envelope", "|");

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceAttack), 1 }, "Attack",
		linRange(0.001f, 10.f, 0.001f, 0.5f), 1.0f, " s"));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceDecay), 1 }, "Decay",
		linRange(0.001f, 10.f, 0.001f, 0.5f), 2.0f, " s"));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceSustain), 1 }, "Sustain",
		linRange(0.f, 1.f, 0.01f, 0.5f), 0.5f));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceRelease), 1 }, "Release",
		linRange(0.001f, 10.f, 0.001f, 0.5f), 1.0f, " s"));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceAttackPower), 1 }, "Attack Power",
		linRange(0.1f, 10.f, 0.01f, 0.25f), 2.0f));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceDecayPower), 1 }, "Decay Power",
		linRange(0.1f, 10.f, 0.01f, 0.25f), 3.0f));

	voiceEnvGroup->addChild(std::make_unique<AudioParameterFloat>(
		ParameterID{ toChars(ID::voiceReleasePower), 1 }, "Release Power",
		linRange(0.1f, 10.f, 0.01f, 0.25f), 4.0f));
	
	layout.add(std::move(voiceEnvGroup));

    return layout;
}

// ──────────────────────────────────────────────────────────────────────────
// 2)  Non-exposed (internal) atomics
// ──────────────────────────────────────────────────────────────────────────
std::unordered_map<std::string, std::atomic<float>>
ParameterCreator::createNonExposed()
{
    using Map = std::unordered_map<std::string, std::atomic<float>>;
    Map map;
	//map.emplace(masterGain, 0.0f);
    return map;
}

// ──────────────────────────────────────────────────────────────────────────
// 3)  Modulation atomics
// ──────────────────────────────────────────────────────────────────────────
std::unordered_map<juce::String, std::atomic<float>>
ParameterCreator::createModFloatMap()
{
	/*
    constexpr const char* modulatable[] = {

    };
	*/

    std::unordered_map<juce::String, std::atomic<float>> map;
	/* Disabled
    for (auto* id : modulatable)
        map.emplace(juce::String(id) + "_mod", 0.0f);
	*/
    return map;
}
