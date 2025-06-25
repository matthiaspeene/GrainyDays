#pragma once

#include <JuceHeader.h>
#include "ParameterSlider.h"

//==============================================================================
/*
*/
class GrainSpawnProperties  : public juce::Component
{
public:
	GrainSpawnProperties(juce::AudioProcessorValueTreeState& apvts);
    ~GrainSpawnProperties() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

	ParameterSlider attackSlider;
	ParameterSlider sustainSlider;
	ParameterSlider releaseSlider;
	ParameterSlider attackCurveSlider;
	ParameterSlider releaseCurveSlider;

	//ParameterSlider delayRandomSlider;
	//ParameterSlider grainRateSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainSpawnProperties)
};
