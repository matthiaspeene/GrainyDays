#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
#pragma region Constructor & Setup

RainAudioProcessor::RainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

RainAudioProcessor::~RainAudioProcessor() = default;

void RainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    parameterBank.loadFromManager(parameterManager);
    engine.setParameterBank(&parameterBank);
    engine.prepare(sampleRate, samplesPerBlock);
}

void RainAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RainAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    auto main = layouts.getMainOutputChannelSet();
    if (main != juce::AudioChannelSet::mono() &&
        main != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (main != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

#pragma endregion

//==============================================================================
#pragma region Main Processing

void RainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear unused output channels
    const int numInput = getTotalNumInputChannels();
    const int numOutput = getTotalNumOutputChannels();
    for (int i = numInput; i < numOutput; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    engine.process(buffer, midi);
    applyLimiter(buffer);
}

#pragma endregion

//==============================================================================
#pragma region Limiter

inline float softLimit(float x)
{
    if (x < -2.0f) return -2.0f;
    if (x > 2.0f) return  2.0f;

    float absX = std::abs(x);
    if (absX <= 1.0f) return x;

    float compressed = 1.0f + (absX - 1.0f) * 0.5f;  // 2:1 compression
    return std::copysign(compressed, x);
}

void RainAudioProcessor::applyLimiter(juce::AudioBuffer<float>& buffer)
{
    const int numCh = buffer.getNumChannels();
    const int numSamp = buffer.getNumSamples();

    for (int ch = 0; ch < numCh; ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamp; ++i)
            data[i] = softLimit(data[i]);
    }
}

#pragma endregion

//==============================================================================
#pragma region Plugin Metadata

const juce::String RainAudioProcessor::getName() const { return JucePlugin_Name; }
bool RainAudioProcessor::acceptsMidi() const { return JucePlugin_WantsMidiInput; }
bool RainAudioProcessor::producesMidi() const { return JucePlugin_ProducesMidiOutput; }
bool RainAudioProcessor::isMidiEffect() const { return JucePlugin_IsMidiEffect; }
double RainAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int RainAudioProcessor::getNumPrograms() { return 1; }
int RainAudioProcessor::getCurrentProgram() { return 0; }
void RainAudioProcessor::setCurrentProgram(int) {}
const juce::String RainAudioProcessor::getProgramName(int) { return {}; }
void RainAudioProcessor::changeProgramName(int, const juce::String&) {}

#pragma endregion

//==============================================================================
#pragma region State & Editor

bool RainAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* RainAudioProcessor::createEditor()
{
    return new RainAudioProcessorEditor(*this);
}

// Save
void RainAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree root("RAIN_STATE");

    // 1) exposed parameters
    root.addChild(parameterManager.getAPVTS().copyState(), -1, nullptr);

    // 2) internal engine parameters
    root.addChild(parameterManager.serialiseInternals(), -1, nullptr);

    // write to the binary block JUCE expects
    juce::MemoryOutputStream mos(destData, false);
    root.writeToStream(mos);
}

// Load
void RainAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ValueTree root = juce::ValueTree::readFromData(data, sizeInBytes);
    if (!root.isValid())
        return;         // guard against corrupt data

    // 1) APVTS
    if (auto params = root.getChildWithName("PARAMETERS"); params.isValid())
    {
        parameterManager.getAPVTS().replaceState(params);
        // the Parameter objects are reused, so no map rebuild is needed
    }

    // 2) internal engine params
    if (auto intern = root.getChildWithName("INTERNALS"); intern.isValid())
        parameterManager.deserialiseInternals(intern);
}


#pragma endregion

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RainAudioProcessor();
}
