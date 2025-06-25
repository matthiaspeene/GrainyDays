#include <JuceHeader.h>
#include "GrainParams.h"
#include "ParameterSlider.h"
#include "ParameterIDs.h"

using namespace ParamID;

//==============================================================================
GrainParams::GrainParams(juce::AudioProcessorValueTreeState& apvts)
	: grainPitchSlider(apvts, toChars(ID::grainPitchMin), toChars(ID::grainPitchMax), juce::Slider::TwoValueHorizontal, true, "Pitch"),
	//grainFineSlider(apvts, toChars(ID::grainFine), juce::Slider::RotaryHorizontalVerticalDrag, true, "Fine"),
	grainGainSlider(apvts, toChars(ID::grainVolumeMin), toChars(ID::grainVolumeMax), juce::Slider::TwoValueHorizontal, true, "Gain"),
	grainPanSlider(apvts, toChars(ID::grainPanMin), toChars(ID::grainPanMax), juce::Slider::TwoValueHorizontal, true, "Pan")
	//grainPitchRandomSlider(apvts, toChars(ID::grainPitchRandom), juce::Slider::RotaryHorizontalVerticalDrag, true, "Pitch Random"),
	//grainFineRandomSlider(apvts, toChars(ID::grainFineRandom), juce::Slider::RotaryHorizontalVerticalDrag, true, "Fine Random"),
	//grainGainRandomSlider(apvts, toChars(ID::grainVolumeRandom), juce::Slider::RotaryHorizontalVerticalDrag, true, "Gain Random"),
	//grainPanRandomSlider(apvts, toChars(ID::grainPanRandom), juce::Slider::RotaryHorizontalVerticalDrag, true, "Pan Random")
{
	addAndMakeVisible(grainPitchSlider);
	//addAndMakeVisible(grainFineSlider);
	addAndMakeVisible(grainGainSlider);
	addAndMakeVisible(grainPanSlider);

	//addAndMakeVisible(grainPitchRandomSlider);
	//addAndMakeVisible(grainFineRandomSlider);
	//addAndMakeVisible(grainGainRandomSlider);
	//addAndMakeVisible(grainPanRandomSlider);
	// Set the random sliders to invisible and not on top
	//grainPitchRandomSlider.setVisible(false);
	//grainPitchRandomSlider.setAlwaysOnTop(false);
	//grainGainRandomSlider.setVisible(false);
	//grainGainRandomSlider.setAlwaysOnTop(false);
	//grainPanRandomSlider.setVisible(false);
	//grainPanRandomSlider.setAlwaysOnTop(false);
	//grainFineRandomSlider.setVisible(false);
	//grainFineRandomSlider.setAlwaysOnTop(false);
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

	static const int sliderHeight = (bounds.getHeight() - 20)/3; // Leave some space for the top line
	static const int sliderWidth = bounds.getWidth()-12; // Four sliders, each taking 1/4 of the width
	grainPitchSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	//grainPitchRandomSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	//grainFineSlider.setBounds(grainPitchSlider.getRight(), 26, sliderWidth, sliderHeight);
	//grainFineRandomSlider.setBounds(grainPitchRandomSlider.getRight(), 26, sliderWidth, sliderHeight);
	grainGainSlider.setBounds(12, grainPitchSlider.getBottom(), sliderWidth, sliderHeight);
	//grainGainRandomSlider.setBounds(grainPitchRandomSlider.getRight(), 26, sliderWidth, sliderHeight);
	grainPanSlider.setBounds(12, grainGainSlider.getBottom(), sliderWidth, sliderHeight);
	//grainPanRandomSlider.setBounds(grainGainRandomSlider.getRight(), 26, sliderWidth, sliderHeight);
}
