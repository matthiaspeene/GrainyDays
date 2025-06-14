/*
  ==============================================================================

    GrainParams.h
    Created: 14 Jun 2025 8:00:18pm
    Author:  M8T

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GrainParams  : public juce::Component
{
public:
    GrainParams();
    ~GrainParams() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainParams)
};
