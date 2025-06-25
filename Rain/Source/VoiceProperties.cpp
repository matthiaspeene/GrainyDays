#include <JuceHeader.h>
#include "VoiceProperties.h"
#include "ParameterIDs.h"

using namespace ParamID;

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

	// Title on top left
	g.setFont(juce::Font(16.0f, juce::Font::bold));
	g.setColour(juce::Colours::black);
	g.drawText("Voice ADSR", 10, 5, getWidth() - 20, 20, juce::Justification::left);
}

void VoiceProperties::resized()
{

}
