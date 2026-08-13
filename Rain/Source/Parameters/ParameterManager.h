#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <unordered_map>
#include <atomic>
#include "ParameterCreator.h"

class ParameterManager
{
public:
    explicit ParameterManager(juce::AudioProcessor&);

    // ─── public queries ────────────────────────────────────────────────────
    std::atomic<float>* getRawParameterValue(const juce::String& id);
    std::atomic<float>* getInternalFloat(const std::string& id);
    std::atomic<float>* getFloatMod(const juce::String& id);

    juce::ValueTree   serialiseInternals()   const;
    void              deserialiseInternals(const juce::ValueTree&);

    juce::RangedAudioParameter* getParameter(const juce::String& id);
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    void buildMaps(); // helper to fill parameterMap

    juce::AudioProcessorValueTreeState apvts;

    std::unordered_map<juce::String, juce::RangedAudioParameter*> parameterMap;

    std::unordered_map<std::string, std::atomic<float>> internalFloatParams;
    std::unordered_map<juce::String, std::atomic<float>> modFloatParams;
};
