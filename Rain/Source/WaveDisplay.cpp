#include "WaveDisplay.h"

WaveDisplay::WaveDisplay() {}

void WaveDisplay::paint(juce::Graphics& g)
{
	// draw a rounded rectangle background
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 20.0f, 2.0f);

    if (currentSample.buffer && currentSample.buffer->getNumSamples() > 0)
    {
        drawWaveform(g, *currentSample.buffer);
    }
    else
    {
        g.setColour(juce::Colours::black);
        g.setFont(20.0f);
        g.drawFittedText("Drag audio file here", getLocalBounds(), juce::Justification::centred, 1);
    }
}

bool WaveDisplay::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files.size() == 1 && files[0].endsWithIgnoreCase(".wav");
}

void WaveDisplay::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.size() == 1)
        loadFile(juce::File(files[0]));
}

void WaveDisplay::setOnAudioLoaded(AudioLoadedCallback callback)
{
    onAudioLoaded = std::move(callback);
}

void WaveDisplay::loadFile(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        auto newBuffer = std::make_shared<juce::AudioBuffer<float>>(static_cast<int>(reader->numChannels),
            static_cast<int>(reader->lengthInSamples));
        reader->read(newBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);

        currentSample.buffer = newBuffer;
        currentSample.sampleRate = reader->sampleRate;

        repaint();

        if (onAudioLoaded)
            onAudioLoaded(currentSample);
    }
}

void WaveDisplay::drawWaveform(juce::Graphics& g, const juce::AudioBuffer<float>& buffer)
{
    const int width = getWidth();
    const int height = getHeight() - 24;
    const int numSamples = buffer.getNumSamples();
    const float* samples = buffer.getReadPointer(0); // Mono or just first channel

    g.setColour(juce::Colours::black);
    juce::Path waveform;
    waveform.startNewSubPath(0, height / 2);

    for (int x = 0; x < width; ++x)
    {
        int sampleIndex = juce::jmap(x, 0, width, 0, numSamples);
        float sample = samples[sampleIndex];
        float y = juce::jmap(sample, -1.0f, 1.0f, (float)height, 24.0f);
        waveform.lineTo((float)x, y);
    }

    g.strokePath(waveform, juce::PathStrokeType(1.5f));
}
