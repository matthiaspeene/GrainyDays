#pragma once

#include <JuceHeader.h>
#include <melatonin_perfetto/melatonin_perfetto.h> // for performance tracing
#include "GrainEngine.h"
#include "ParameterManager.h"
#include "ParameterBank.h"

class RainAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RainAudioProcessor();
    ~RainAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	GrainEngine& getEngine() { return engine; }
	ParameterManager& getParameterManager() { return parameterManager; }

private:
	// ------------------------------------------------------ Functions
    void applyLimiter(juce::AudioBuffer<float>& buffer);

    // ------------------------------------------------------ parameters (UI)
    ParameterManager parameterManager{ *this };   // owns the APVTS the host sees
    ParameterBank    parameterBank;                // cached atomic<float>* bundle

    // ------------------------------------------------------ real-time DSP
    GrainEngine      engine;                       // the granular synth core

#if PERFETTO
    MelatoninPerfetto tracingSession;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RainAudioProcessor)
};