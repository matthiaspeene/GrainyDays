#include "WaveDisplay.h"
#include "ParameterIDs.h"

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

        sampleBuffer.store(newBuf, std::memory_order_release);

        currentSample.buffer = newBuf;
        currentSample.sampleRate = reader->sampleRate;

        juce::MessageManager::callAsync([this] { repaint(); });

        if (onAudioLoaded) onAudioLoaded(currentSample);
        startPosSlider.setVisible(true);
    }
}

void WaveDisplay::drawWaveform(juce::Graphics& g,
    const juce::AudioBuffer<float>& buffer)
{
    const int w = getWidth(), h = getHeight() - 24;
    const int n = buffer.getNumSamples();
    const float* samples = buffer.getReadPointer(0);

    juce::Path path;  path.startNewSubPath(0, h * 0.5f);

    for (int x = 0; x < w; ++x)
    {
        int si = juce::jlimit(0, n - 1, juce::jmap(x, 0, w, 0, n));  // bounds-safe
        float y = juce::jmap(samples[si], -1.0f, 1.0f, (float)h, 24.0f);
        path.lineTo((float)x, y);
    }

    g.setColour(juce::Colours::black);
    g.strokePath(path, juce::PathStrokeType(1.5f));
}
