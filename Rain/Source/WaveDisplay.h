#pragma once

#include <JuceHeader.h>
#include "LoadedSample.h"

class WaveDisplay : public juce::Component,
    public juce::FileDragAndDropTarget
{
public:
    using AudioLoadedCallback = std::function<void(const LoadedSample&)>;

    WaveDisplay();
    ~WaveDisplay() override = default;

    void paint(juce::Graphics&) override;
    void resized() override {}

    // Drag and drop
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void setOnAudioLoaded(AudioLoadedCallback callback);

private:
    void loadFile(const juce::File& file);
    void drawWaveform(juce::Graphics& g, const juce::AudioBuffer<float>& buffer);

    LoadedSample currentSample;
    AudioLoadedCallback onAudioLoaded;
};
