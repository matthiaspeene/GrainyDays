#pragma once

#include <JuceHeader.h>
#include "ParameterSlider.h"

class GrainParams  : public juce::Component
{
public:
	GrainParams(juce::AudioProcessorValueTreeState& apvts);
    ~GrainParams() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

	bool keyPressed(const juce::KeyPress& k) override;

    ParameterSlider grainPitchSlider;
	//ParameterSlider grainFineSlider;
	ParameterSlider grainGainSlider;
	ParameterSlider grainPanSlider;

	/*
	ParameterSlider grainPitchRandomSlider;
	//ParameterSlider grainFineRandomSlider;
	ParameterSlider grainGainRandomSlider;
	ParameterSlider grainPanRandomSlider;
	*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainParams)
};
