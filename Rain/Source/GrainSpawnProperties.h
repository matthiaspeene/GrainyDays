#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GrainSpawnProperties  : public juce::Component
{
public:
    GrainSpawnProperties();
    ~GrainSpawnProperties() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainSpawnProperties)
};
