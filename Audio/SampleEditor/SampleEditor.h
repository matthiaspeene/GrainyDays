/*
  ==============================================================================

    SampleEditor.h - A JUCE-based editor window for permanent sample editing
    This provides similar functionality to the WaveDisplay but with
    the ability to save permanent copies and additional editing features.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Rain/Source/LoadedSample.h"

namespace BetterUEAudio
{
namespace SampleEditor
{

class SampleEditor : public juce::Component,
                     public juce::FileDragAndDropTarget,
                     public juce::Button::Listener,
                     public juce::Slider::Listener
{
public:
    using SampleSavedCallback = std::function<void(const LoadedSample&, const juce::File&)>;

    SampleEditor();
    ~SampleEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Drag and drop
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;

    // Callbacks
    void setOnSampleSaved(SampleSavedCallback callback);

    // Get current sample
    const LoadedSample& getCurrentSample() const { return currentSample; }

private:
    // Core functionality
    void loadFile(const juce::File& file);
    void saveCurrentSample();
    void updateProcessedSample();
    void drawWaveform(juce::Graphics& g, const juce::AudioBuffer<float>& buffer);
    void drawPositionMarkers(juce::Graphics& g);

    // Sample processing
    void applyAllProcessing();
    juce::AudioBuffer<float> createProcessedBuffer();
    void applyPitchShift(juce::AudioBuffer<float>& buffer);
    void applySampleRateChange(juce::AudioBuffer<float>& buffer);
    void createSeamlessLoop(juce::AudioBuffer<float>& buffer);

    // UI components
    juce::Slider startPosSlider;
    juce::Slider endPosSlider;
    juce::ToggleButton loopButton;
    juce::Slider pitchSlider;
    juce::Slider sampleRateSlider;
    juce::TextButton saveButton;
    juce::TextButton loadButton;

    juce::Label startPosLabel;
    juce::Label endPosLabel;
    juce::Label loopLabel;
    juce::Label pitchLabel;
    juce::Label sampleRateLabel;
    juce::Label titleLabel;

    // Sample data
    LoadedSample originalSample;
    LoadedSample currentSample;
    
    // Atomic buffer for thread-safe access
    std::atomic<std::shared_ptr<juce::AudioBuffer<float>>> displayBuffer{ nullptr };

    // Parameters
    float startPosition = 0.0f;  // 0.0 to 1.0
    float endPosition = 1.0f;    // 0.0 to 1.0
    bool loopEnabled = false;
    float pitchShift = 0.0f;     // in semitones
    float sampleRateMultiplier = 1.0f;

    // Callbacks
    SampleSavedCallback onSampleSaved;

    // Helper for getting thread-safe buffer
    [[nodiscard]] std::shared_ptr<const juce::AudioBuffer<float>> getCurrentBuffer() const noexcept
    {
        return displayBuffer.load(std::memory_order_acquire);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleEditor)
};

class SampleEditorWindow : public juce::DocumentWindow
{
public:
    SampleEditorWindow(const juce::String& name);
    ~SampleEditorWindow() override = default;

    void closeButtonPressed() override;

    SampleEditor& getEditor() { return *editor; }

private:
    std::unique_ptr<SampleEditor> editor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleEditorWindow)
};

} // namespace SampleEditor
} // namespace BetterUEAudio