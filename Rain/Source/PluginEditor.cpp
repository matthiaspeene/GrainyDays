/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RainAudioProcessorEditor::RainAudioProcessorEditor (RainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(p.getParameterManager().getAPVTS())
{
	waveformDisplay = std::make_unique<WaveDisplay>();
    waveformDisplay->setOnAudioLoaded([this](const LoadedSample& sample)
        {
            DBG("Loaded audio with " << sample.buffer->getNumSamples()
                << " samples at " << sample.sampleRate << " Hz");

			audioProcessor.getEngine().setLoadedSample(sample);
			gSampleSize = sample.buffer->getNumSamples();
        });

	grainVisualizer = std::make_unique<GrainVisualizer>();
	grainSpawnProperties = std::make_unique<GrainSpawnProperties>();
	voiceProperties = std::make_unique<VoiceProperties>();
	grainParams = std::make_unique<GrainParams>();
	grainMods = std::make_unique<GrainMods>();

	addAndMakeVisible(*waveformDisplay);
	addAndMakeVisible(*grainVisualizer);
	addAndMakeVisible(*grainSpawnProperties);
	addAndMakeVisible(*voiceProperties);
	addAndMakeVisible(*grainParams);
	addAndMakeVisible(*grainMods);

    setSize (900, 656);
}

RainAudioProcessorEditor::~RainAudioProcessorEditor()
{
}

//==============================================================================
void RainAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colour(0xFF787878));
}

void RainAudioProcessorEditor::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(12, 12);
	auto workspaceHeight = bounds.getHeight()-24;
	auto grainVisualizerHeight = workspaceHeight / 2.0f;
	auto waveDisplayBounds = bounds.removeFromTop(grainVisualizerHeight);
	waveformDisplay->setBounds(waveDisplayBounds);
	grainVisualizer->setBounds(waveDisplayBounds);

	bounds.removeFromTop(12);
	auto workWidth = bounds.getWidth() - 12;
	// Left colum
	auto collumWidth = workWidth / 2.0f;
	auto leftColumn = bounds.removeFromLeft(collumWidth);

	grainSpawnProperties->setBounds(leftColumn.removeFromTop(leftColumn.getHeight()/1.75-12));
	leftColumn.removeFromTop(12); // Add some space between spawn properties and voice properties
	grainParams->setBounds(leftColumn.removeFromLeft(leftColumn.getWidth()/1.5-12));
	leftColumn.removeFromLeft(12); // Add some space between grain params and grain mods
	grainMods->setBounds(leftColumn);

	// Right column
	auto rightColumn = bounds.removeFromRight(collumWidth);
	voiceProperties->setBounds(rightColumn.removeFromTop(rightColumn.getHeight() / 1.75 - 12));

	// Bottom right reserved for future expansions
}
