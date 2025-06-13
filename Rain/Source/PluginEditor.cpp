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

    setSize (570, 300);
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
	auto bounds = getLocalBounds();
	// top of the screen for waveform display
	auto waveDisplayBounds = bounds.removeFromTop(bounds.getHeight() / 2.5);
	waveformDisplay->setBounds(waveDisplayBounds);
	grainVisualizer->setBounds(waveDisplayBounds);
	addAndMakeVisible(*waveformDisplay);
	addAndMakeVisible(*grainVisualizer);
	startPosSlider.setBounds(bounds.removeFromTop(15));
	startPosRandomSlider.setBounds(bounds.removeFromTop(15));
	addAndMakeVisible(startPosSlider);
	addAndMakeVisible(startPosRandomSlider);

	bounds.removeFromTop(10);
}
