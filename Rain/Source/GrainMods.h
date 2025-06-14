#pragma once
#include <JuceHeader.h>

class GrainMods : public juce::Component
{
public:
    GrainMods();
    ~GrainMods() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainMods)
};