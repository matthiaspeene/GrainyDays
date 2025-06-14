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
#include "GrainMods.h"
#include "GrainSpawnProperties.h"
#include "VoiceProperties.h"
#include "GrainParams.h"



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
	std::unique_ptr<GrainSpawnProperties> grainSpawnProperties;
	std::unique_ptr<VoiceProperties> voiceProperties;
	std::unique_ptr<GrainParams> grainParams;
	std::unique_ptr<GrainMods> grainMods;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessorEditor)
};
