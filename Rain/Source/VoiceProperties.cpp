/*
  ==============================================================================

    VoiceProperties.cpp
    Created: 14 Jun 2025 8:01:05pm
    Author:  M8T

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VoiceProperties.h"

//==============================================================================
VoiceProperties::VoiceProperties()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

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
}

void VoiceProperties::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
