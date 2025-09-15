/*
  ==============================================================================

    SampleEditor.cpp - Implementation of the SampleEditor class
    Provides functionality for permanent sample editing with start/end positions,
    seamless looping, pitch shifting, and sample rate changes.

  ==============================================================================
*/

#include "SampleEditor.h"

namespace BetterUEAudio
{
namespace SampleEditor
{

//==============================================================================
SampleEditor::SampleEditor()
{
    // Setup title
    titleLabel.setText("Sample Editor", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Setup start position slider
    startPosSlider.setRange(0.0, 1.0, 0.001);
    startPosSlider.setValue(0.0);
    startPosSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    startPosSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    startPosSlider.addListener(this);
    addAndMakeVisible(startPosSlider);

    startPosLabel.setText("Start Position", juce::dontSendNotification);
    startPosLabel.attachToComponent(&startPosSlider, true);
    addAndMakeVisible(startPosLabel);

    // Setup end position slider
    endPosSlider.setRange(0.0, 1.0, 0.001);
    endPosSlider.setValue(1.0);
    endPosSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    endPosSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    endPosSlider.addListener(this);
    addAndMakeVisible(endPosSlider);

    endPosLabel.setText("End Position", juce::dontSendNotification);
    endPosLabel.attachToComponent(&endPosSlider, true);
    addAndMakeVisible(endPosLabel);

    // Setup loop button
    loopButton.setButtonText("Seamless Loop");
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    // Setup pitch slider
    pitchSlider.setRange(-12.0, 12.0, 0.1);
    pitchSlider.setValue(0.0);
    pitchSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);

    pitchLabel.setText("Pitch (Semitones)", juce::dontSendNotification);
    pitchLabel.attachToComponent(&pitchSlider, true);
    addAndMakeVisible(pitchLabel);

    // Setup sample rate slider
    sampleRateSlider.setRange(0.5, 2.0, 0.01);
    sampleRateSlider.setValue(1.0);
    sampleRateSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sampleRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    sampleRateSlider.addListener(this);
    addAndMakeVisible(sampleRateSlider);

    sampleRateLabel.setText("Sample Rate Multiplier", juce::dontSendNotification);
    sampleRateLabel.attachToComponent(&sampleRateSlider, true);
    addAndMakeVisible(sampleRateLabel);

    // Setup buttons
    loadButton.setButtonText("Load Sample");
    loadButton.addListener(this);
    addAndMakeVisible(loadButton);

    saveButton.setButtonText("Save Processed Sample");
    saveButton.addListener(this);
    saveButton.setEnabled(false);
    addAndMakeVisible(saveButton);
}

void SampleEditor::paint(juce::Graphics& g)
{
    // Draw background
    g.fillAll(juce::Colour(0xFF2D2D2D));
    
    // Draw waveform area background
    auto waveformArea = getLocalBounds().removeFromTop(getHeight() / 2);
    waveformArea.reduce(10, 10);
    
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(waveformArea.toFloat(), 8.0f);
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(waveformArea.toFloat(), 8.0f, 2.0f);

    // Draw waveform or placeholder
    if (auto buf = getCurrentBuffer(); buf && buf->getNumSamples() > 0)
    {
        g.setColour(juce::Colours::black);
        drawWaveform(g, *buf);
        drawPositionMarkers(g);
    }
    else
    {
        g.setColour(juce::Colours::grey);
        g.setFont(18.0f);
        g.drawFittedText("Drop audio file here or click Load Sample", 
                        waveformArea, juce::Justification::centred, 1);
    }
}

void SampleEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Title at top
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(10);
    
    // Waveform display area (top half)
    auto waveformArea = bounds.removeFromTop(bounds.getHeight() / 2);
    bounds.removeFromTop(20);
    
    // Controls area (bottom half)
    auto controlsArea = bounds;
    controlsArea.reduce(20, 10);
    
    // Position controls
    auto positionArea = controlsArea.removeFromTop(60);
    auto startArea = positionArea.removeFromLeft(positionArea.getWidth() / 2);
    startArea.removeFromLeft(120); // Space for label
    startPosSlider.setBounds(startArea.reduced(10));
    
    positionArea.removeFromLeft(120); // Space for label
    endPosSlider.setBounds(positionArea.reduced(10));
    
    controlsArea.removeFromTop(20);
    
    // Loop button
    loopButton.setBounds(controlsArea.removeFromTop(30).reduced(100, 0));
    controlsArea.removeFromTop(20);
    
    // Pitch and sample rate controls
    auto modArea = controlsArea.removeFromTop(60);
    auto pitchArea = modArea.removeFromLeft(modArea.getWidth() / 2);
    pitchArea.removeFromLeft(140); // Space for label
    pitchSlider.setBounds(pitchArea.reduced(10));
    
    modArea.removeFromLeft(140); // Space for label
    sampleRateSlider.setBounds(modArea.reduced(10));
    
    controlsArea.removeFromTop(20);
    
    // Buttons
    auto buttonArea = controlsArea.removeFromTop(40);
    loadButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(20, 0));
    saveButton.setBounds(buttonArea.reduced(20, 0));
}

bool SampleEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files.size() == 1 && (files[0].endsWithIgnoreCase(".wav") || 
                                files[0].endsWithIgnoreCase(".aif") ||
                                files[0].endsWithIgnoreCase(".aiff") ||
                                files[0].endsWithIgnoreCase(".flac"));
}

void SampleEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.size() == 1 && juce::File(files[0]).existsAsFile())
        loadFile(juce::File(files[0]));
}

void SampleEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio file to load",
                                 juce::File{},
                                 "*.wav;*.aif;*.aiff;*.flac");
        
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            loadFile(file);
        }
    }
    else if (button == &saveButton)
    {
        saveCurrentSample();
    }
    else if (button == &loopButton)
    {
        loopEnabled = loopButton.getToggleState();
        updateProcessedSample();
    }
}

void SampleEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &startPosSlider)
    {
        startPosition = (float)startPosSlider.getValue();
        // Ensure start position is before end position
        if (startPosition >= endPosition)
        {
            endPosition = juce::jmin(1.0f, startPosition + 0.01f);
            endPosSlider.setValue(endPosition, juce::dontSendNotification);
        }
    }
    else if (slider == &endPosSlider)
    {
        endPosition = (float)endPosSlider.getValue();
        // Ensure end position is after start position
        if (endPosition <= startPosition)
        {
            startPosition = juce::jmax(0.0f, endPosition - 0.01f);
            startPosSlider.setValue(startPosition, juce::dontSendNotification);
        }
    }
    else if (slider == &pitchSlider)
    {
        pitchShift = (float)pitchSlider.getValue();
    }
    else if (slider == &sampleRateSlider)
    {
        sampleRateMultiplier = (float)sampleRateSlider.getValue();
    }
    
    updateProcessedSample();
    repaint();
}

void SampleEditor::setOnSampleSaved(SampleSavedCallback callback)
{
    onSampleSaved = std::move(callback);
}

void SampleEditor::loadFile(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        auto newBuffer = std::make_shared<juce::AudioBuffer<float>>((int)reader->numChannels,
                                                                   (int)reader->lengthInSamples);
        reader->read(newBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
        
        originalSample.buffer = newBuffer;
        originalSample.sampleRate = reader->sampleRate;
        
        // Reset controls to defaults
        startPosition = 0.0f;
        endPosition = 1.0f;
        pitchShift = 0.0f;
        sampleRateMultiplier = 1.0f;
        loopEnabled = false;
        
        startPosSlider.setValue(startPosition, juce::dontSendNotification);
        endPosSlider.setValue(endPosition, juce::dontSendNotification);
        pitchSlider.setValue(pitchShift, juce::dontSendNotification);
        sampleRateSlider.setValue(sampleRateMultiplier, juce::dontSendNotification);
        loopButton.setToggleState(loopEnabled, juce::dontSendNotification);
        
        updateProcessedSample();
        saveButton.setEnabled(true);
        
        juce::MessageManager::callAsync([this] { repaint(); });
    }
}

void SampleEditor::saveCurrentSample()
{
    if (!currentSample.buffer || currentSample.buffer->getNumSamples() == 0)
        return;
        
    juce::FileChooser chooser("Save processed sample",
                             juce::File{},
                             "*.wav");
    
    if (chooser.browseForFileToSave(true))
    {
        auto file = chooser.getResult();
        
        // Create audio format writer
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        
        auto* format = formatManager.findFormatForFileExtension(file.getFileExtension());
        if (format != nullptr)
        {
            std::unique_ptr<juce::FileOutputStream> fileStream(file.createOutputStream());
            if (fileStream != nullptr)
            {
                std::unique_ptr<juce::AudioFormatWriter> writer(
                    format->createWriterFor(fileStream.get(),
                                          currentSample.sampleRate,
                                          currentSample.buffer->getNumChannels(),
                                          24, // 24-bit
                                          {},
                                          0));
                if (writer != nullptr)
                {
                    fileStream.release(); // Writer now owns the stream
                    writer->writeFromAudioSampleBuffer(*currentSample.buffer,
                                                     0,
                                                     currentSample.buffer->getNumSamples());
                    
                    if (onSampleSaved)
                        onSampleSaved(currentSample, file);
                }
            }
        }
    }
}

void SampleEditor::updateProcessedSample()
{
    if (!originalSample.buffer || originalSample.buffer->getNumSamples() == 0)
        return;
    
    auto processedBuffer = createProcessedBuffer();
    auto sharedBuffer = std::make_shared<juce::AudioBuffer<float>>(std::move(processedBuffer));
    
    currentSample.buffer = sharedBuffer;
    currentSample.sampleRate = originalSample.sampleRate * sampleRateMultiplier;
    
    displayBuffer.store(sharedBuffer, std::memory_order_release);
}

juce::AudioBuffer<float> SampleEditor::createProcessedBuffer()
{
    if (!originalSample.buffer)
        return {};
    
    const auto& original = *originalSample.buffer;
    int originalLength = original.getNumSamples();
    
    // Calculate start and end samples
    int startSample = (int)(startPosition * originalLength);
    int endSample = (int)(endPosition * originalLength);
    int length = endSample - startSample;
    
    if (length <= 0)
        return {};
    
    // Create cropped buffer
    juce::AudioBuffer<float> processedBuffer(original.getNumChannels(), length);
    
    for (int channel = 0; channel < original.getNumChannels(); ++channel)
    {
        processedBuffer.copyFrom(channel, 0, original, channel, startSample, length);
    }
    
    // Apply seamless loop if enabled
    if (loopEnabled)
    {
        createSeamlessLoop(processedBuffer);
    }
    
    // Apply pitch shift if needed
    if (std::abs(pitchShift) > 0.01f)
    {
        applyPitchShift(processedBuffer);
    }
    
    return processedBuffer;
}

void SampleEditor::createSeamlessLoop(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() < 2)
        return;
    
    // Simple crossfade approach for seamless looping
    int fadeLength = juce::jmin(buffer.getNumSamples() / 10, 1000); // 10% or max 1000 samples
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        // Crossfade the end with the beginning
        for (int i = 0; i < fadeLength; ++i)
        {
            float fade = (float)i / fadeLength;
            int endIdx = buffer.getNumSamples() - fadeLength + i;
            
            channelData[endIdx] = channelData[endIdx] * (1.0f - fade) + channelData[i] * fade;
        }
    }
}

void SampleEditor::applyPitchShift(juce::AudioBuffer<float>& buffer)
{
    // Simple pitch shifting using interpolation (basic implementation)
    // For production use, consider using more sophisticated algorithms like PSOLA or phase vocoder
    
    float pitchRatio = std::pow(2.0f, pitchShift / 12.0f);
    int originalLength = buffer.getNumSamples();
    int newLength = (int)(originalLength / pitchRatio);
    
    juce::AudioBuffer<float> tempBuffer(buffer.getNumChannels(), newLength);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        const auto* sourceData = buffer.getReadPointer(channel);
        auto* destData = tempBuffer.getWritePointer(channel);
        
        for (int i = 0; i < newLength; ++i)
        {
            float sourcePos = i * pitchRatio;
            int sourceIndex = (int)sourcePos;
            float fraction = sourcePos - sourceIndex;
            
            if (sourceIndex < originalLength - 1)
            {
                destData[i] = sourceData[sourceIndex] * (1.0f - fraction) + 
                             sourceData[sourceIndex + 1] * fraction;
            }
            else if (sourceIndex < originalLength)
            {
                destData[i] = sourceData[sourceIndex];
            }
        }
    }
    
    buffer = std::move(tempBuffer);
}

void SampleEditor::drawWaveform(juce::Graphics& g, const juce::AudioBuffer<float>& buffer)
{
    auto waveformArea = getLocalBounds().removeFromTop(getHeight() / 2);
    waveformArea.reduce(10, 10);
    
    const int w = waveformArea.getWidth();
    const int h = waveformArea.getHeight();
    const int n = buffer.getNumSamples();
    
    if (n == 0 || w == 0 || h == 0)
        return;
    
    const float* samples = buffer.getReadPointer(0);
    
    juce::Path path;
    path.startNewSubPath(waveformArea.getX(), waveformArea.getCentreY());
    
    for (int x = 0; x < w; ++x)
    {
        int sampleIndex = juce::jlimit(0, n - 1, juce::jmap(x, 0, w, 0, n));
        float y = juce::jmap(samples[sampleIndex], -1.0f, 1.0f, 
                           (float)waveformArea.getBottom(), (float)waveformArea.getY());
        path.lineTo((float)(waveformArea.getX() + x), y);
    }
    
    g.setColour(juce::Colours::blue);
    g.strokePath(path, juce::PathStrokeType(1.5f));
}

void SampleEditor::drawPositionMarkers(juce::Graphics& g)
{
    auto waveformArea = getLocalBounds().removeFromTop(getHeight() / 2);
    waveformArea.reduce(10, 10);
    
    // Draw start position marker
    int startX = waveformArea.getX() + (int)(startPosition * waveformArea.getWidth());
    g.setColour(juce::Colours::green);
    g.drawVerticalLine(startX, waveformArea.getY(), waveformArea.getBottom());
    g.drawText("Start", startX - 20, waveformArea.getY() - 20, 40, 20, 
              juce::Justification::centred, true);
    
    // Draw end position marker
    int endX = waveformArea.getX() + (int)(endPosition * waveformArea.getWidth());
    g.setColour(juce::Colours::red);
    g.drawVerticalLine(endX, waveformArea.getY(), waveformArea.getBottom());
    g.drawText("End", endX - 20, waveformArea.getY() - 20, 40, 20, 
              juce::Justification::centred, true);
}

//==============================================================================
SampleEditorWindow::SampleEditorWindow(const juce::String& name)
    : DocumentWindow(name, juce::Colours::grey, DocumentWindow::allButtons)
{
    editor = std::make_unique<SampleEditor>();
    setContentOwned(editor.get(), true);
    setResizable(true, true);
    setSize(800, 600);
    setTopLeftPosition(100, 100);
}

void SampleEditorWindow::closeButtonPressed()
{
    setVisible(false);
}

} // namespace SampleEditor
} // namespace BetterUEAudio