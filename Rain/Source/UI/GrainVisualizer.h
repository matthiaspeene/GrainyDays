#pragma once
#include <JuceHeader.h>
#include "GrainVisualData.h"

// ---------------------------------------------------------------- GrainVisualizer
class GrainVisualizer : public juce::Component,
    private juce::Timer
{
public:
    explicit GrainVisualizer(GrainVisualData& visualDataToUse);
    ~GrainVisualizer() override = default;

private:
    // juce::Component ---------------------------------------------------------
    void paint(juce::Graphics& g) override;

    // juce::Timer -------------------------------------------------------------
    void timerCallback() override;

    GrainVisualData& visualData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer)
};
