#pragma once

#include <JuceHeader.h>

struct LoadedSample
{
    std::shared_ptr<juce::AudioBuffer<float>> buffer;
    double sampleRate = 44100.0; // fallback if unknown
};