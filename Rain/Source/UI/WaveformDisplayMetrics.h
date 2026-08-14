#pragma once

#include <JuceHeader.h>

namespace waveformDisplay
{
inline constexpr float horizontalInset = 12.0f;
inline constexpr float grainMarkerDiameter = 10.0f;

inline juce::Rectangle<float> getSampleBounds(juce::Rectangle<int> componentBounds) noexcept
{
    return componentBounds.toFloat().reduced(horizontalInset, 0.0f);
}
}
