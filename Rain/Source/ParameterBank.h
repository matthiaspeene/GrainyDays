#pragma once
#include <atomic>

// A bundle of pointers cached once on the message thread, 
// then read cheaply from the audio thread.
struct ParameterBank
{
	// Global parameters
    std::atomic<float>* masterGain = nullptr;
    std::atomic<float>* grainDensity = nullptr;

	// Voice Parameters
	std::atomic<float>* midiRootNote = nullptr;

	// Grain parameters
    std::atomic<float>* grainPitch = nullptr;
    std::atomic<float>* grainVolume = nullptr;
	std::atomic<float>* grainPan = nullptr;
	std::atomic<float>* grainLength = nullptr; // Lenght in seconds
	std::atomic<float>* grainPosition = nullptr;

	// Filter parameters
    std::atomic<float>* filterCutoff = nullptr;
    std::atomic<float>* filterResonance = nullptr;


    // ---------------------------------------------------------------------
    // One-shot linker (call in prepareToPlay or constructor of your engine)
    void loadFromManager(class ParameterManager& mgr);
};
