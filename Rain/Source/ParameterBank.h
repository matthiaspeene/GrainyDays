#pragma once
#include <atomic>

// A bundle of pointers cached once on the message thread, 
// then read cheaply from the audio thread.
struct ParameterBank
{
	// Gyro Param
	std::atomic<float>* gyroStrength = nullptr;

	// Global parameters
    std::atomic<float>* masterGain = nullptr;
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
    std::atomic<float>* filterCutoff = nullptr;
    std::atomic<float>* filterResonance = nullptr;




    // ---------------------------------------------------------------------
    // One-shot linker (call in prepareToPlay or constructor of your engine)
    void loadFromManager(class ParameterManager& mgr);
};
