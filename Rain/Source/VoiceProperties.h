#pragma once

#include <JuceHeader.h>
#include "ParameterSlider.h"
//==============================================================================
/*
*/
class VoiceProperties  : public juce::Component
{
public:
    VoiceProperties(juce::AudioProcessorValueTreeState& apvts);
    ~VoiceProperties() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

	ParameterSlider attackSlider;
	ParameterSlider decaySlider;
	ParameterSlider sustainSlider;
	ParameterSlider releaseSlider;

	ParameterSlider attackPowerSlider;
	ParameterSlider decayPowerSlider;
	ParameterSlider releasePowerSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceProperties)
};
