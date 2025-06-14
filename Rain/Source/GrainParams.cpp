#include <JuceHeader.h>
#include "GrainParams.h"

//==============================================================================
GrainParams::GrainParams()
{

}

GrainParams::~GrainParams()
{
}

void GrainParams::paint (juce::Graphics& g)
{
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
}

void GrainParams::resized()
{

}
