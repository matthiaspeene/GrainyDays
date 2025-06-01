#include "ParameterBank.h"
#include "ParameterManager.h"
#include "ParameterIDs.h"

void ParameterBank::loadFromManager(ParameterManager& mgr)
{
    using namespace ParamID;

	playMode = mgr.getRawParameterValue(ParamID::playMode);

	//gyroStrength = mgr.getRawParameterValue(ParamID::gyroStrength);

    //masterGain = mgr.getRawParameterValue(ParamID::masterGain);
    grainDensity = mgr.getRawParameterValue(ParamID::grainDensity);
	delayRandomRange = mgr.getRawParameterValue(ParamID::delayRandomRange);

	midiRootNote = mgr.getRawParameterValue(ParamID::midiRootNote);
    grainPitch = mgr.getRawParameterValue(ParamID::grainPitch);
	grainPitchRandomRange = mgr.getRawParameterValue(ParamID::grainPitchRandomRange);
    grainVolume = mgr.getRawParameterValue(ParamID::grainVolume);
	grainVolumeRandomRange = mgr.getRawParameterValue(ParamID::grainVolumeRandomRange);
	grainPan = mgr.getRawParameterValue(ParamID::grainPan);
	grainPanRandomRange = mgr.getRawParameterValue(ParamID::grainPanRandomRange);
	grainPosition = mgr.getRawParameterValue(ParamID::grainPosition);
	grainPositionRandomRange = mgr.getRawParameterValue(ParamID::grainPositionRandomRange);

    //filterCutoff = mgr.getRawParameterValue(ParamID::filterCutoff);
    //filterResonance = mgr.getRawParameterValue(ParamID::filterResonance);

	envAttack = mgr.getRawParameterValue(ParamID::envAttack);
	envSustainLength = mgr.getRawParameterValue(ParamID::envSustainLength);
	envRelease = mgr.getRawParameterValue(ParamID::envRelease);
	envAttackCurve = mgr.getRawParameterValue(ParamID::envAttackCurve);
	envReleaseCurve = mgr.getRawParameterValue(ParamID::envReleaseCurve);

	velocityModGrainDensity = mgr.getRawParameterValue(ParamID::velocityModGrainDensity);
	velocityModGrainPitch = mgr.getRawParameterValue(ParamID::velocityModGrainPitch);
	velocityModGrainVolume = mgr.getRawParameterValue(ParamID::velocityModGrainVolume);
	velocityModGrainPan = mgr.getRawParameterValue(ParamID::velocityModGrainPan);
	velocityModGrainPosition = mgr.getRawParameterValue(ParamID::velocityModGrainPosition);

	rotZModGrainDensity = mgr.getRawParameterValue(ParamID::rotZModGrainDensity);
	rotZModGrainPitch = mgr.getRawParameterValue(ParamID::rotZModGrainPitch);
	rotZModGrainVolume = mgr.getRawParameterValue(ParamID::rotZModGrainVolume);
	rotZModGrainPan = mgr.getRawParameterValue(ParamID::rotZModGrainPan);
	rotZModGrainPosition = mgr.getRawParameterValue(ParamID::rotZModGrainPosition);
}
