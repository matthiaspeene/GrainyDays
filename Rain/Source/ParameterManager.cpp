#include "ParameterManager.h"
#include "ParameterIDs.h"

// ──────────────────────────────────────────────────────────────────────────
ParameterManager::ParameterManager(juce::AudioProcessor& processor)
    : apvts(processor, nullptr, "PARAMETERS", ParameterCreator::createLayout())
{
    internalFloatParams = ParameterCreator::createNonExposed();
    modFloatParams = ParameterCreator::createModFloatMap();
    buildMaps();
}

// ──────────────────────────────────────────────────────────────────────────
void ParameterManager::buildMaps()
{
    parameterMap.clear();
    for (const auto& n : apvts.state)
        if (auto* p = apvts.getParameter(n.getProperty("id").toString()))
            parameterMap[n.getProperty("id").toString()] = p;
}

// ──────────────────────────────────────────────────────────────────────────
juce::RangedAudioParameter* ParameterManager::getParameter(const juce::String& id)
{
    if (auto it = parameterMap.find(id); it != parameterMap.end())
        return it->second;
    jassertfalse; return nullptr;
}

std::atomic<float>* ParameterManager::getRawParameterValue(const juce::String& id)
{
    if (auto* p = apvts.getRawParameterValue(id))
        return p;
    jassertfalse; return nullptr;
}

std::atomic<float>* ParameterManager::getInternalFloat(const std::string& id)
{
    if (auto it = internalFloatParams.find(id); it != internalFloatParams.end())
        return &it->second;
    jassertfalse; return nullptr;
}

std::atomic<float>* ParameterManager::getFloatMod(const juce::String& id)
{
    if (auto it = modFloatParams.find(id + "_mod"); it != modFloatParams.end())
        return &it->second;
    jassertfalse; return nullptr;
}
