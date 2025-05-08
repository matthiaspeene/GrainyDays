#include "ParameterBank.h"
#include "ParameterManager.h"
#include "ParameterIDs.h"

void ParameterBank::loadFromManager(ParameterManager& mgr)
{
    using namespace ParamID;
    masterGain = mgr.getRawParameterValue(ParamID::masterGain);
    grainDensity = mgr.getRawParameterValue(ParamID::grainDensity);

	midiRootNote = mgr.getRawParameterValue(ParamID::midiRootNote);
    grainPitch = mgr.getRawParameterValue(ParamID::grainPitch);
    grainVolume = mgr.getRawParameterValue(ParamID::grainVolume);
	grainPan = mgr.getRawParameterValue(ParamID::grainPan);
	grainPosition = mgr.getRawParameterValue(ParamID::grainPosition);

    filterCutoff = mgr.getRawParameterValue(ParamID::filterCutoff);
    filterResonance = mgr.getRawParameterValue(ParamID::filterResonance);

	envAttack = mgr.getRawParameterValue(ParamID::envAttack);
	envSustainLength = mgr.getRawParameterValue(ParamID::envSustainLength);
	envRelease = mgr.getRawParameterValue(ParamID::envRelease);
	envAttackCurve = mgr.getRawParameterValue(ParamID::envAttackCurve);
	envReleaseCurve = mgr.getRawParameterValue(ParamID::envReleaseCurve);
}
