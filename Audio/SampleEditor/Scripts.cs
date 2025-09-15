/*
  ==============================================================================

    Scripts.cs - Main header for the BetterUEAudio.SampleEditor namespace
    This file contains the core SampleEditor class definitions and provides
    an editor window with similar functionality to the scriptable object
    but with the ability to save permanent copies.

    Features:
    - Setting new start and end positions on audio files
    - Creating seamless loops
    - Repitching samples
    - Changing sample rate
    - Saving permanent copies of processed samples

  ==============================================================================
*/

#pragma once

// Include the main SampleEditor implementation
#include "SampleEditor.h"

namespace BetterUEAudio
{
namespace SampleEditor
{
    // Main editor class for sample editing with permanent save functionality
    // This replaces the temporary preview functionality of the scriptable object
    // and provides similar controls for:
    // - Start/End position editing
    // - Seamless loop creation
    // - Pitch shifting (repitching)
    // - Sample rate adjustment
    // - Permanent sample saving
    using SampleEditor = SampleEditor;
    
    // Window wrapper for the editor
    using SampleEditorWindow = SampleEditorWindow;
    
    // Utility function to create and show a new sample editor window
    inline std::unique_ptr<SampleEditorWindow> createSampleEditorWindow()
    {
        auto window = std::make_unique<SampleEditorWindow>("Sample Editor");
        window->setVisible(true);
        return window;
    }
    
} // namespace SampleEditor
} // namespace BetterUEAudio

/*
  Usage Example:
  
  #include "Audio/SampleEditor/Scripts.cs"
  
  // Create a new sample editor window
  auto editorWindow = BetterUEAudio::SampleEditor::createSampleEditorWindow();
  
  // Set up callback for when samples are saved
  editorWindow->getEditor().setOnSampleSaved([](const LoadedSample& sample, const juce::File& file) {
      DBG("Sample saved to: " << file.getFullPathName());
  });
*/