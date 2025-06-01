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
#include "ParameterRangeSlider.h"
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

	//juce::GroupComponent positionGroup{ "positionGroup", "Grain Position" };
	ParameterRangeSlider startPosSlider{ apvts, ParamID::grainPosition, ParamID::grainPositionRandomRange };
	juce::Label startPosLabel{ "startPosLabel", "Position" };
	ParameterSlider startPosVelocityModSlider{ apvts, ParamID::velocityModGrainPosition, juce::Slider::LinearBar, false};
	juce::Label startPosVelocityModLabel{ "startPosVelocityModLabel", "Velocity Mod" };
	ParameterSlider startPosRotationModSlider{ apvts, ParamID::rotZModGrainPosition, juce::Slider::LinearBar, false };
	juce::Label startPosRotationModLabel{ "startPosRotationModLabel", "Rotation Mod" };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessorEditor)
};
