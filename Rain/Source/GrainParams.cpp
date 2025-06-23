#include <JuceHeader.h>
#include "GrainParams.h"
#include "ParameterSlider.h"
#include "ParameterIDs.h"

using namespace ParamID;

//==============================================================================
GrainParams::GrainParams(juce::AudioProcessorValueTreeState& apvts)
	: grainPitchSlider(apvts, toChars(ID::grainPitch), juce::Slider::RotaryHorizontalVerticalDrag, true, "Pitch"),
	//grainFineSlider(apvts, toChars(ID::grainFine), juce::Slider::RotaryHorizontalVerticalDrag, true, "Fine"),
	grainGainSlider(apvts, toChars(ID::grainVolume), juce::Slider::RotaryHorizontalVerticalDrag, true, "Gain"),
	grainPanSlider(apvts, toChars(ID::grainPan), juce::Slider::RotaryHorizontalVerticalDrag, true, "Pan")
{
	addAndMakeVisible(grainPitchSlider);
	//addAndMakeVisible(grainFineSlider);
	addAndMakeVisible(grainGainSlider);
	addAndMakeVisible(grainPanSlider);
}

GrainParams::~GrainParams()
{
}

void GrainParams::paint (juce::Graphics& g)
{
	// Background with rounded corners
	g.setColour(juce::Colours::lightgrey.brighter());
	// Expanded bounds to move the bottom edge outside the component so they aren't rounded
	auto bounds = juce::Rectangle<float>(0,-20,getWidth()+20, getHeight()+20);
	g.fillRoundedRectangle(bounds, 20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(bounds, 20.0f, 2.0f);
	// Draw line on the top
	g.drawLine(0, 0, getWidth(), 0, 2.0f);
	// Draw line on the right
	g.drawLine(getWidth(), 0, getWidth(), getHeight(), 2.0f);

	// Title on top left
	g.setFont(juce::Font(16.0f, juce::Font::bold));
	g.setColour(juce::Colours::black);
	g.drawText("Grain Parameters", 10, 5, getWidth() - 20, 20, juce::Justification::left);
}

void GrainParams::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12, 6);

	static const int sliderHeight = bounds.getHeight() - 20; // Leave some space for the top line
	static const int sliderWidth = bounds.getWidth() / 4; // Four sliders, each taking 1/4 of the width
	grainPitchSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	//grainFineSlider.setBounds(grainPitchSlider.getRight(), 26, sliderWidth, sliderHeight);
	grainGainSlider.setBounds(grainPitchSlider.getRight(), 26, sliderWidth, sliderHeight);
	grainPanSlider.setBounds(grainGainSlider.getRight(), 26, sliderWidth, sliderHeight);
}
