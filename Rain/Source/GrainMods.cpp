#include "GrainMods.h"
#include "ParameterIDs.h"

using namespace ParamID;

GrainMods::GrainMods(juce::AudioProcessorValueTreeState& apvts)
	: delayRandomSlider(apvts, toChars(ID::delayRandomRange), juce::Slider::LinearVertical, true, "Delay RND"),
	grainRateSlider(apvts, toChars(ID::grainRate), juce::Slider::RotaryHorizontalVerticalDrag, true, "Rate")
{
	addAndMakeVisible(delayRandomSlider);
	addAndMakeVisible(grainRateSlider);
	resized();
}


GrainMods::~GrainMods()
{
}

void GrainMods::paint(juce::Graphics& g)
{
	// Draw a rounded rectangle with a straight bottom
	g.setColour(juce::Colours::lightgrey.brighter());
	// Background
	auto bounds = juce::Rectangle<float>(-20, -20, getWidth()+20, getHeight()+20);
	g.fillRoundedRectangle(bounds, 20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(bounds, 20.0f, 2.0f);
	// Straight line at the top
	g.drawLine(0, 0, getWidth(), 0, 2.0f); // Straight top line
	// Straight line on the left
	g.drawLine(0, 0, 0, getHeight(), 2.0f); // Straight left line

	// Title on top left
	g.setFont(juce::Font(16.0f, juce::Font::bold));
	g.setColour(juce::Colours::black);
	g.drawText("Grain Spawning", 10, 5, getWidth() - 20, 20, juce::Justification::left);
}

void GrainMods::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12,6);
	bounds.removeFromTop(20); // Remove space for title
	delayRandomSlider.setBounds(bounds.removeFromRight(bounds.getWidth()/2));
	grainRateSlider.setBounds(bounds);
}
