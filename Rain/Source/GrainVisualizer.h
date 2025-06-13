#pragma once
#include <JuceHeader.h>
#include "GlobalVariables.h"
#include "GrainVisualData.h"

// ---------------------------------------------------------------- GrainVisualizer
class GrainVisualizer : public juce::Component,
    private juce::Timer
{
public:
    GrainVisualizer();
    ~GrainVisualizer() override = default;

private:
    // juce::Component ---------------------------------------------------------
    void paint(juce::Graphics& g) override;

    // juce::Timer -------------------------------------------------------------
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer)
};
