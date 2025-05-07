/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveDisplay.h"

//==============================================================================
/**
*/
class RainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RainAudioProcessorEditor (RainAudioProcessor&);
    ~RainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RainAudioProcessor& audioProcessor;

	// Waveform display
	std::unique_ptr<WaveDisplay> waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessorEditor)
};
