/*
  ==============================================================================

    VoiceProperties.h
    Created: 14 Jun 2025 8:01:05pm
    Author:  M8T

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class VoiceProperties  : public juce::Component
{
public:
    VoiceProperties();
    ~VoiceProperties() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceProperties)
};
