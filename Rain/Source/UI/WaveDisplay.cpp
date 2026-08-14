#include "WaveDisplay.h"
#include "../Parameters/ParameterIDs.h"
#include "WaveformDisplayMetrics.h"

using namespace ParamID;

WaveDisplay::WaveDisplay(juce::AudioProcessorValueTreeState& apvts) 
    : startPosSlider(apvts, toChars(ID::grainPositionMin), toChars(ID::grainPositionMax))
{
	addAndMakeVisible(startPosSlider);
    startPosSlider.setVisible(false);
}

void WaveDisplay::paint(juce::Graphics& g)
{
	// draw a rounded rectangle background
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 20.0f, 2.0f);
	g.setFont(20.0f);

    if (auto buf = getCurrentBuffer(); buf && buf->getNumSamples() > 0)
        drawWaveform(g, *buf);
    else
        g.drawFittedText("Drag audio file here", getLocalBounds(),
            juce::Justification::centred, 1);
}

void WaveDisplay::resized()
{
	auto bounds = getLocalBounds();
	startPosSlider.setBounds(bounds.removeFromBottom(60)); // Place the slider at the bottom
}

bool WaveDisplay::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files.size() == 1 && files[0].endsWithIgnoreCase(".wav");
}

void WaveDisplay::filesDropped(const juce::StringArray& files, int, int)
{
    startPosSlider.setVisible(false);

    if (files.size() == 1 && juce::File(files[0]).existsAsFile())
        loadFile(juce::File(files[0]));
	else
        repaint();
}

void WaveDisplay::setOnAudioLoaded(AudioLoadedCallback callback)
{
    onAudioLoaded = std::move(callback);
}

void WaveDisplay::setSample(const LoadedSample& sample)
{
    currentSample = sample;
    sampleBuffer.store(sample.buffer, std::memory_order_release);
    startPosSlider.setVisible(sample.buffer != nullptr && sample.buffer->getNumSamples() > 0);
    repaint();
}

void WaveDisplay::loadFile(const juce::File& file)
{
    juce::AudioFormatManager fm;  
    fm.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(file));
    if (reader)
    {
        auto newBuf = std::make_shared<juce::AudioBuffer<float>>((int)reader->numChannels,
            (int)reader->lengthInSamples);
        reader->read(newBuf.get(), 0, (int)reader->lengthInSamples, 0, true, true);

        setSample({ newBuf, reader->sampleRate, file.getFullPathName() });

        if (onAudioLoaded) onAudioLoaded(currentSample);
    }
}

void WaveDisplay::drawWaveform(juce::Graphics& g,
    const juce::AudioBuffer<float>& buffer)
{
    const int n = buffer.getNumSamples();
    if (n <= 0)
        return;

    const auto sampleBounds = waveformDisplay::getSampleBounds(getLocalBounds());
    const int displayWidth = juce::jmax(1, juce::roundToInt(sampleBounds.getWidth()));
    const float top = 24.0f;
    const float bottom = static_cast<float>(getHeight() - 60);
    const float* samples = buffer.getReadPointer(0);

    juce::Path path;
    path.startNewSubPath(sampleBounds.getX(), (top + bottom) * 0.5f);

    for (int pixel = 0; pixel <= displayWidth; ++pixel)
    {
        const float proportion = static_cast<float>(pixel) / static_cast<float>(displayWidth);
        const int sampleIndex = juce::jlimit(0, n - 1,
            juce::roundToInt(proportion * static_cast<float>(n - 1)));
        const float x = sampleBounds.getX() + proportion * sampleBounds.getWidth();
        const float y = juce::jmap(samples[sampleIndex], -1.0f, 1.0f, bottom, top);
        path.lineTo(x, y);
    }

    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType(1.5f));
}
