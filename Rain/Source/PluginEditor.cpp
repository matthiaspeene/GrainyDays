/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RainAudioProcessorEditor::RainAudioProcessorEditor (RainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(p.getParameterManager().getAPVTS())
{
	waveformDisplay = std::make_unique<WaveDisplay>();
    waveformDisplay->setOnAudioLoaded([this](const LoadedSample& sample)
        {
            DBG("Loaded audio with " << sample.buffer->getNumSamples()
                << " samples at " << sample.sampleRate << " Hz");

			audioProcessor.getEngine().setLoadedSample(sample);
			gSampleSize = sample.buffer->getNumSamples();
        });

	grainVisualizer = std::make_unique<GrainVisualizer>();

	startPosLabel.attachToComponent(&startPosSlider, true);
	startPosVelocityModLabel.attachToComponent(&startPosVelocityModSlider, true);
	startPosRotationModLabel.attachToComponent(&startPosRotationModSlider, true);
	addAndMakeVisible(startPosLabel);
	addAndMakeVisible(startPosVelocityModLabel);
	addAndMakeVisible(startPosRotationModLabel);

	addAndMakeVisible(startPosSlider);
	addAndMakeVisible(startPosVelocityModSlider);
	addAndMakeVisible(startPosRotationModSlider);

	addAndMakeVisible(*waveformDisplay);
	addAndMakeVisible(*grainVisualizer);

    setSize (1420, 600);
}

RainAudioProcessorEditor::~RainAudioProcessorEditor()
{
}

//==============================================================================
void RainAudioProcessorEditor::paint (juce::Graphics& g)
{

}

void RainAudioProcessorEditor::resized()
{
	auto bounds = getLocalBounds().reduced(4);
	// top of the screen for waveform display
	auto waveDisplayBounds = bounds.removeFromTop(bounds.getHeight() / 2.5);
	auto leftOfWaveBounds =	waveDisplayBounds.removeFromLeft(waveDisplayBounds.getWidth()/4); //
	waveformDisplay->setBounds(waveDisplayBounds);
	grainVisualizer->setBounds(waveDisplayBounds);

	//  leftOfWaveBounds == Put dropdown for playmode, rootnote numberbox and and density sliders here: TopLeft: Playmode TopRight: rootnote. // Labels on left
	// BottomLeft : Grain Density + ModVelocity and ModRotation sliders + BottomRight is Delay Random Range slider. DelayRandomRange: RadialSlider with label on top, Grain Density + ModVelocity and ModRotation sliders can be 3 horizontal bars with labels on the left

	bounds.removeFromTop(4); // tiny space between wave display and controls
	startPosSlider.setBounds(bounds.removeFromTop(16).removeFromRight(bounds.getWidth() * 3 / 4)); // Match with waveDisplay bounds
	bounds.removeFromTop(2); // tiny space between sliders

	auto twoSliderBelow = bounds.removeFromTop(12);
	auto sliderLeftBelow = twoSliderBelow.removeFromLeft(twoSliderBelow.getWidth() / 2);
	auto sliderRightBelow = twoSliderBelow; // remaining bounds = right side 
	sliderLeftBelow.removeFromLeft(120); // leave space for label
	sliderRightBelow.removeFromLeft(120); // leave space for label

	startPosVelocityModSlider.setBounds(sliderLeftBelow);
	startPosRotationModSlider.setBounds(sliderRightBelow);

	bounds.removeFromTop(6);

	auto leftBounds = bounds.removeFromLeft(bounds.getWidth() / 2); // Left half for pitch and volume controls
	auto rightBounds = bounds; // Right half for pan controls
	auto topLeft = leftBounds.removeFromTop(leftBounds.getHeight() / 2); // Top half for pitch controls
	auto bottomLeft = leftBounds; // Bottom half for volume controls
	auto topRight = rightBounds.removeFromTop(rightBounds.getHeight() / 2); // Top half for pan controls
	auto bottomRight = rightBounds; // Bottom half is empty

	// Top Left: Pitch controls. Pitch, Pitch Random Range, ModRotation, ModVelocity. Use radial sliders with title labels on top
	// Top Right: Volume controls. Volume, Volume Random Range, ModRotation, ModVelocity. use radial sliders with title labels on top
	// BottomLeft: Pan controls. Pan, Pan Random Range, ModRotation, ModVelocity. use radial sliders with title labels on top
	// BottomRight: Empty for now

}
