#include <JuceHeader.h>
#include "GrainSpawnProperties.h"
#include "ParameterIDs.h"

using namespace ParamID;

//==============================================================================
GrainSpawnProperties::GrainSpawnProperties(juce::AudioProcessorValueTreeState& apvts)
	: attackSlider(apvts, toChars(ID::envAttack), juce::Slider::RotaryHorizontalVerticalDrag, true, "Attack"),
	sustainSlider(apvts, toChars(ID::envSustainLength), juce::Slider::RotaryHorizontalVerticalDrag, true, "Sustain"),
	releaseSlider(apvts, toChars(ID::envRelease), juce::Slider::RotaryHorizontalVerticalDrag, true, "Release"),
	attackCurveSlider(apvts, toChars(ID::envAttackCurve), juce::Slider::LinearVertical, true, "P"),
	releaseCurveSlider(apvts, toChars(ID::envReleaseCurve), juce::Slider::LinearVertical, true, "P")
{
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(attackCurveSlider);
	addAndMakeVisible(releaseCurveSlider);
}

GrainSpawnProperties::~GrainSpawnProperties()
{
}

void GrainSpawnProperties::paint (juce::Graphics& g)
{
	// Draw a rounded rectangle with a straight bottom
	g.setColour(juce::Colours::lightgrey.brighter());
	// Background
	// Expanded bounds to move the bottom edge outside the component so they aren't rounded
	auto bounds = juce::Rectangle<float>(getWidth(), getHeight()+20);
	g.fillRoundedRectangle(bounds,20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(bounds, 20.0f, 2.0f);
	g.drawLine(0, getHeight(), getWidth(), getHeight(), 2.0f); // Straight bottom line

	// Title on top left
	g.setFont(juce::Font(16.0f, juce::Font::bold));
	g.setColour(juce::Colours::black);
	g.drawText("Grainshape", 10, 5, getWidth() - 20, 20, juce::Justification::left);
}

void GrainSpawnProperties::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12, 6);

	static const int sliderHeight = bounds.getHeight() - 20; // Leave some space for the top line
	static const int sliderWidth = bounds.getWidth() / 4;

	attackCurveSlider.setBounds(12, 26, sliderWidth / 2, sliderHeight);
	attackSlider.setBounds(attackCurveSlider.getRight(), 26, sliderWidth, sliderHeight);
	sustainSlider.setBounds(attackSlider.getRight(), 26, sliderWidth, sliderHeight);
	releaseSlider.setBounds(sustainSlider.getRight(), 26, sliderWidth, sliderHeight);
	releaseCurveSlider.setBounds(releaseSlider.getRight(), 26, sliderWidth/2, sliderHeight);
}
