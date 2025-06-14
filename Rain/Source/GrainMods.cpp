#include "GrainMods.h"

GrainMods::GrainMods()
{
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
}

void GrainMods::resized()
{
}
