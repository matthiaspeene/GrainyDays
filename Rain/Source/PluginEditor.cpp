/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
RainAudioProcessorEditor::RainAudioProcessorEditor (RainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	waveformDisplay = std::make_unique<WaveDisplay>();
    waveformDisplay->setOnAudioLoaded([this](const LoadedSample& sample)
        {
            DBG("Loaded audio with " << sample.buffer->getNumSamples()
                << " samples at " << sample.sampleRate << " Hz");

			audioProcessor.getEngine().setLoadedSample(sample);
        });


    setSize (400, 300);
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
    waveformDisplay->setBounds(getLocalBounds());
    addAndMakeVisible(*waveformDisplay);
}
