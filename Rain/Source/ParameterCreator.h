#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <unordered_map>
#include <atomic>

class ParameterCreator
{
public:
    // Layout for the APVTS
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    // Map of paramID → atomic<float> for hidden runtime values
    static std::unordered_map<std::string, std::atomic<float>> createNonExposed();

    // Map of "paramID_mod" → atomic<float> for modulation buses
    static std::unordered_map<juce::String, std::atomic<float>> createModFloatMap();
};
