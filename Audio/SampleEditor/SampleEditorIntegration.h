/*
  ==============================================================================

    SampleEditorIntegration.h - Integration example for the SampleEditor
    Shows how to integrate the SampleEditor with the existing Rain plugin

  ==============================================================================
*/

#pragma once

#include "Scripts.cs"
#include "../../Rain/Source/PluginEditor.h"

namespace BetterUEAudio
{
namespace SampleEditor
{

class SampleEditorIntegration
{
public:
    static void addSampleEditorToPlugin(RainAudioProcessorEditor& pluginEditor)
    {
        // Create a button to open the sample editor
        auto* openEditorButton = new juce::TextButton("Open Sample Editor");
        openEditorButton->onClick = [&pluginEditor]()
        {
            // Create and show the sample editor window
            auto editorWindow = createSampleEditorWindow();
            
            // Set up callback for when samples are saved
            editorWindow->getEditor().setOnSampleSaved([&pluginEditor](const LoadedSample& sample, const juce::File& file)
            {
                DBG("Sample saved to: " << file.getFullPathName());
                
                // Optionally load the saved sample into the main plugin
                // This would require exposing the engine's setLoadedSample method
                // pluginEditor.getAudioProcessor().getEngine().setLoadedSample(sample);
            });
            
            // Keep the window alive (in a real implementation, you'd manage this properly)
            static std::vector<std::unique_ptr<SampleEditorWindow>> openWindows;
            openWindows.push_back(std::move(editorWindow));
        };
        
        // Add the button to the plugin editor (this would need proper layout management)
        pluginEditor.addAndMakeVisible(openEditorButton);
    }
    
    // Example of how to create a standalone sample editor application
    static int runStandaloneSampleEditor()
    {
        juce::ScopedJuceInitialiser_GUI libraryInitialiser;
        
        auto editorWindow = createSampleEditorWindow();
        editorWindow->setVisible(true);
        
        // Simple message loop for standalone app
        juce::MessageManager::getInstance()->runDispatchLoop();
        
        return 0;
    }
};

} // namespace SampleEditor
} // namespace BetterUEAudio

/*
  Usage in main plugin:
  
  #include "Audio/SampleEditor/SampleEditorIntegration.h"
  
  // In RainAudioProcessorEditor constructor:
  BetterUEAudio::SampleEditor::SampleEditorIntegration::addSampleEditorToPlugin(*this);
  
  // For standalone sample editor:
  int main()
  {
      return BetterUEAudio::SampleEditor::SampleEditorIntegration::runStandaloneSampleEditor();
  }
*/