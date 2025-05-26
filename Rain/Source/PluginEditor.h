/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveDisplay.h"
#include "GrainVisualizer.h"



class RainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RainAudioProcessorEditor (RainAudioProcessor&);
    ~RainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    RainAudioProcessor& audioProcessor;

	// Components
	std::unique_ptr<WaveDisplay> waveformDisplay;
	std::unique_ptr<GrainVisualizer> grainVisualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessorEditor)
};
