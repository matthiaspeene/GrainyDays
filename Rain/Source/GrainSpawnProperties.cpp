#include <JuceHeader.h>
#include "GrainSpawnProperties.h"

//==============================================================================
GrainSpawnProperties::GrainSpawnProperties()
{

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
}

void GrainSpawnProperties::resized()
{

}
