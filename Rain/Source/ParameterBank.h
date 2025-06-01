#pragma once
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;

// A bundle of pointers cached once on the message thread, 
// then read cheaply from the audio thread.
struct ParameterBank
{
	std::atomic<float>* playMode = nullptr; // 0 = Midi, 1 = Gyro, 2 = Rotation

	// Gyro Param
	//std::atomic<float>* gyroStrength = nullptr;

	// Global parameters
    //std::atomic<float>* masterGain = nullptr;
    std::atomic<float>* grainDensity = nullptr;
	std::atomic<float>* delayRandomRange = nullptr; // in seconds

	// Voice Parameters
	std::atomic<float>* midiRootNote = nullptr;

	// Grain parameters
	std::atomic<float>* grainPitch = nullptr; // in semitones
	std::atomic<float>* grainPitchRandomRange = nullptr; // in semitones
    std::atomic<float>* grainVolume = nullptr;
	std::atomic<float>* grainVolumeRandomRange = nullptr; // in dB
	std::atomic<float>* grainPan = nullptr;
	std::atomic<float>* grainPanRandomRange = nullptr; // in %
	std::atomic<float>* grainPosition = nullptr; // Position in %
	std::atomic<float>* grainPositionRandomRange = nullptr; // in %

	// Env
	std::atomic<float>* envAttack = nullptr; // in seconds
	std::atomic<float>* envSustainLength = nullptr; // in seconds
	std::atomic<float>* envRelease = nullptr; // in seconds
	std::atomic<float>* envAttackCurve = nullptr; // Power
	std::atomic<float>* envReleaseCurve = nullptr; // Power
    
    // Filter parameters
    //std::atomic<float>* filterCutoff = nullptr;
    //std::atomic<float>* filterResonance = nullptr;

	std::atomic<float>* velocityModGrainDensity = nullptr; // in grains per second
	std::atomic<float>* velocityModGrainPitch = nullptr; // in semitones
	std::atomic<float>* velocityModGrainVolume = nullptr; // in dB
	std::atomic<float>* velocityModGrainPan = nullptr; // in %
	std::atomic<float>* velocityModGrainPosition = nullptr; // in %

	std::atomic<float>* rotZModGrainDensity = nullptr; // in grains per second
	std::atomic<float>* rotZModGrainPitch = nullptr; // in semitones
	std::atomic<float>* rotZModGrainVolume = nullptr; // in dB
	std::atomic<float>* rotZModGrainPan = nullptr; // in %
	std::atomic<float>* rotZModGrainPosition = nullptr; // in %

	// Modulation parameters
	float velocity; // 0-1
	float rotZ; // 0-1

	// Mods
	float grainDensityMod = 0.0f; // in grains per second
	float grainPitchMod = 0.0f; // in Semitones
	float grainVolumeMod = 0.0f; // in dB
	float grainPanMod = 0.0f; // in %
	float grainPositionMod = 0.0f; // in %

    // ---------------------------------------------------------------------
    // One-shot linker (call in prepareToPlay or constructor of your engine)
    void loadFromManager(class ParameterManager& mgr);
};
