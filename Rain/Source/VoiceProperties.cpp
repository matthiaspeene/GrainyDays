#include <JuceHeader.h>
#include "VoiceProperties.h"
#include "ParameterIDs.h"

using namespace ParamID;

//==============================================================================
VoiceProperties::VoiceProperties(juce::AudioProcessorValueTreeState& apvts)
	: attackSlider(apvts, ParamID::toChars(ID::voiceAttack), juce::Slider::RotaryHorizontalVerticalDrag, true, "Attack"),
	decaySlider(apvts, ParamID::toChars(ID::voiceDecay), juce::Slider::RotaryHorizontalVerticalDrag, true, "Decay"),
	sustainSlider(apvts, ParamID::toChars(ID::voiceSustain), juce::Slider::RotaryHorizontalVerticalDrag, true, "Sustain"),
	releaseSlider(apvts, ParamID::toChars(ID::voiceRelease), juce::Slider::RotaryHorizontalVerticalDrag, true, "Release"),
	attackPowerSlider(apvts, ParamID::toChars(ID::voiceAttackPower), juce::Slider::LinearVertical, true, "Attack Power"),
	decayPowerSlider(apvts, ParamID::toChars(ID::voiceDecayPower), juce::Slider::LinearVertical, true, "Decay Power"),
	releasePowerSlider(apvts, ParamID::toChars(ID::voiceReleasePower), juce::Slider::LinearVertical, true, "Release Power")
{
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(decaySlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(attackPowerSlider);
	addAndMakeVisible(decayPowerSlider);
	addAndMakeVisible(releasePowerSlider);
}

VoiceProperties::~VoiceProperties()
{
}

void VoiceProperties::paint (juce::Graphics& g)
{
	// Draw a rounded rectangle with a straight bottom
	g.setColour(juce::Colours::lightgrey.brighter());
	// Background
	// Expanded bounds to move the bottom edge outside the component so they aren't rounded
	auto bounds = juce::Rectangle<float>(getWidth(), getHeight() + 20);
	g.fillRoundedRectangle(bounds, 20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(bounds, 20.0f, 2.0f);
	g.drawLine(0, getHeight(), getWidth(), getHeight(), 2.0f); // Straight bottom line

	// Title on top left
	g.setFont(juce::Font(16.0f, juce::Font::bold));
	g.setColour(juce::Colours::black);
	g.drawText("Voice ADSR", 10, 5, getWidth() - 20, 20, juce::Justification::left);
}

void VoiceProperties::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12, 6);

	static const int sliderHeight = bounds.getHeight() - 20; // Leave some space for the top line
	static const int sliderWidth = bounds.getWidth() / 5.5; // Normal width for each slider. Some slider count as half a slider width

	attackSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	attackPowerSlider.setBounds(attackSlider.getRight(), 26, sliderWidth / 2, sliderHeight);
	decaySlider.setBounds(attackPowerSlider.getRight(), 26, sliderWidth, sliderHeight);
	decayPowerSlider.setBounds(decaySlider.getRight(), 26, sliderWidth / 2, sliderHeight);
	sustainSlider.setBounds(decayPowerSlider.getRight(), 26, sliderWidth, sliderHeight);
	releaseSlider.setBounds(sustainSlider.getRight(), 26, sliderWidth, sliderHeight);
	releasePowerSlider.setBounds(releaseSlider.getRight(), 26, sliderWidth / 2, sliderHeight);
}
