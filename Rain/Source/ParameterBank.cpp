#include "ParameterBank.h"
#include "ParameterManager.h"
#include "ParameterIDs.h"

void ParameterBank::loadFromManager(ParameterManager& mgr)
{
    using namespace ParamID;
    masterGain = mgr.getRawParameterValue(ParamID::masterGain);
    grainDensity = mgr.getRawParameterValue(ParamID::grainDensity);

    grainPitch = mgr.getRawParameterValue(ParamID::grainPitch);
    grainVolume = mgr.getRawParameterValue(ParamID::grainVolume);
	grainPan = mgr.getRawParameterValue(ParamID::grainPan);
	grainLength = mgr.getRawParameterValue(ParamID::grainLength);
	grainPosition = mgr.getRawParameterValue(ParamID::grainPosition);

    filterCutoff = mgr.getRawParameterValue(ParamID::filterCutoff);
    filterResonance = mgr.getRawParameterValue(ParamID::filterResonance);
}
