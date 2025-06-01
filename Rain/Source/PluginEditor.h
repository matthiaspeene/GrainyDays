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
#include "ParameterSlider.h"
#include "ParameterIDs.h"



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
    juce::AudioProcessorValueTreeState& apvts;

	// Components
	std::unique_ptr<WaveDisplay> waveformDisplay;
	std::unique_ptr<GrainVisualizer> grainVisualizer;

	// Parameter sliders
	ParameterSlider startPosSlider{ apvts, ParamID::grainPosition, juce::Slider::LinearBar, false};
	ParameterSlider startPosRandomSlider{ apvts, ParamID::grainPositionRandomRange, juce::Slider::LinearBar, false };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessorEditor)
};
