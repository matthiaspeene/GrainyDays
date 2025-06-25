#pragma once
#include <JuceHeader.h>
#include "ParameterSlider.h"

class GrainMods : public juce::Component
{
public:
    GrainMods(juce::AudioProcessorValueTreeState& apvts);
    ~GrainMods() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ParameterSlider delayRandomSlider;
    ParameterSlider grainRateSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainMods)
};