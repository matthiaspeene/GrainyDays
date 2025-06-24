#include <JuceHeader.h>
#include "GrainSpawnProperties.h"
#include "ParameterIDs.h"

using namespace ParamID;

//==============================================================================
GrainSpawnProperties::GrainSpawnProperties(juce::AudioProcessorValueTreeState& apvts)
	: attackSlider(apvts, toChars(ID::envAttack), juce::Slider::RotaryHorizontalVerticalDrag, true, "Attack"),
	sustainSlider(apvts, toChars(ID::envSustainLength), juce::Slider::RotaryHorizontalVerticalDrag, true, "Sustain"),
	releaseSlider(apvts, toChars(ID::envRelease), juce::Slider::RotaryHorizontalVerticalDrag, true, "Release"),
	attackCurveSlider(apvts, toChars(ID::envAttackCurve), juce::Slider::RotaryHorizontalVerticalDrag, true, "Attack Curve"),
	releaseCurveSlider(apvts, toChars(ID::envReleaseCurve), juce::Slider::RotaryHorizontalVerticalDrag, true, "Release Curve"),
	delayRandomSlider(apvts, toChars(ID::delayRandomRange), juce::Slider::RotaryHorizontalVerticalDrag, true, "Random Delay"),
	grainRateSlider(apvts, toChars(ID::grainRate), juce::Slider::RotaryHorizontalVerticalDrag, true, "Rate")
{
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);
	addAndMakeVisible(attackCurveSlider);
	addAndMakeVisible(releaseCurveSlider);
	addAndMakeVisible(delayRandomSlider);
	addAndMakeVisible(grainRateSlider);

	// Set the curve sliders to invisible and not on top
	attackCurveSlider.setVisible(false);
	attackCurveSlider.setAlwaysOnTop(false);
	releaseCurveSlider.setVisible(false);
	releaseCurveSlider.setAlwaysOnTop(false);

	delayRandomSlider.setVisible(false);
	delayRandomSlider.setAlwaysOnTop(false);
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
	g.drawText("Spawn", 10, 5, getWidth() - 20, 20, juce::Justification::right);
}

void GrainSpawnProperties::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12, 6);

	static const int sliderHeight = bounds.getHeight() - 20; // Leave some space for the top line
	static const int sliderWidth = bounds.getWidth() / 4;

	attackSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	attackCurveSlider.setBounds(12, 26, sliderWidth, sliderHeight);
	sustainSlider.setBounds(attackSlider.getRight(), 26, sliderWidth, sliderHeight);
	releaseSlider.setBounds(sustainSlider.getRight(), 26, sliderWidth, sliderHeight);
	releaseCurveSlider.setBounds(sustainSlider.getRight(), 26, sliderWidth, sliderHeight);

	// Spawnrate sliders
	grainRateSlider.setBounds(releaseSlider.getRight(), 26, sliderWidth, sliderHeight);
	delayRandomSlider.setBounds(releaseSlider.getRight(), 26, sliderWidth, sliderHeight);
}

bool GrainSpawnProperties::keyPressed(const juce::KeyPress& k)
{
	if (k.getKeyCode() == juce::KeyPress::tabKey)
	{
		// Toggle Slider visibility
		attackSlider.setVisible(!attackSlider.isVisible());
		sustainSlider.setVisible(!sustainSlider.isVisible());
		releaseSlider.setVisible(!releaseSlider.isVisible());
		attackCurveSlider.setVisible(!attackCurveSlider.isVisible());
		releaseCurveSlider.setVisible(!releaseCurveSlider.isVisible());
		delayRandomSlider.setVisible(!delayRandomSlider.isVisible());
		grainRateSlider.setVisible(!grainRateSlider.isVisible());

		// Set the always on top state to match visibility
		attackSlider.setAlwaysOnTop(attackSlider.isVisible());
		sustainSlider.setAlwaysOnTop(sustainSlider.isVisible());
		releaseSlider.setAlwaysOnTop(releaseSlider.isVisible());
		attackCurveSlider.setAlwaysOnTop(attackCurveSlider.isVisible());
		releaseCurveSlider.setAlwaysOnTop(releaseCurveSlider.isVisible());
		delayRandomSlider.setAlwaysOnTop(delayRandomSlider.isVisible());
		grainRateSlider.setAlwaysOnTop(grainRateSlider.isVisible());

		repaint(); // Repaint to reflect changes

		return true; // Handled the key press
	}
	return false; // Not handled
}
