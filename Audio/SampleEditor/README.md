# Sample Editor

## Overview

The SampleEditor provides a dedicated editor window for permanent sample editing, replacing the temporary preview functionality of the previous scriptable object implementation. It is located in the `BetterUEAudio.SampleEditor` namespace as requested.

## Features

The SampleEditor provides the same functionality as the scriptable object but with the ability to save permanent copies:

- **Start/End Position Editing**: Set new start and end positions on audio files using intuitive sliders
- **Seamless Loop Creation**: Enable seamless looping with crossfade functionality to eliminate clicks/pops
- **Repitching**: Adjust pitch in semitones (-12 to +12 semitones)
- **Sample Rate Changes**: Modify sample rate with a multiplier (0.5x to 2.0x)
- **Permanent Saving**: Save processed samples as permanent audio files

## File Structure

```
Audio/
└── SampleEditor/
    ├── Scripts.cs              # Main namespace header (as requested)
    ├── SampleEditor.h          # Header file with class definitions
    ├── SampleEditor.cpp        # Implementation file
    ├── SampleEditorIntegration.h # Integration examples
    └── SampleEditorTest.cpp    # Test file
```

## Usage

### Basic Usage

```cpp
#include "Audio/SampleEditor/Scripts.cs"

// Create a new sample editor window
auto editorWindow = BetterUEAudio::SampleEditor::createSampleEditorWindow();

// Set up callback for when samples are saved
editorWindow->getEditor().setOnSampleSaved([](const LoadedSample& sample, const juce::File& file) {
    DBG("Sample saved to: " << file.getFullPathName());
});
```

### Integration with Existing Plugin

```cpp
#include "Audio/SampleEditor/SampleEditorIntegration.h"

// In your plugin editor constructor:
BetterUEAudio::SampleEditor::SampleEditorIntegration::addSampleEditorToPlugin(*this);
```

## API Reference

### SampleEditor Class

```cpp
namespace BetterUEAudio::SampleEditor
{
    class SampleEditor : public juce::Component
    {
    public:
        SampleEditor();
        
        // Set callback for when samples are saved
        void setOnSampleSaved(SampleSavedCallback callback);
        
        // Get current processed sample
        const LoadedSample& getCurrentSample() const;
        
        // Drag and drop support
        bool isInterestedInFileDrag(const juce::StringArray& files) override;
        void filesDropped(const juce::StringArray& files, int x, int y) override;
    };
}
```

### SampleEditorWindow Class

```cpp
namespace BetterUEAudio::SampleEditor
{
    class SampleEditorWindow : public juce::DocumentWindow
    {
    public:
        SampleEditorWindow(const juce::String& name);
        SampleEditor& getEditor();
    };
}
```

## Controls

1. **Load Sample Button**: Opens file chooser to load audio files
2. **Start Position Slider**: Sets the start position (0.0 to 1.0) of the sample
3. **End Position Slider**: Sets the end position (0.0 to 1.0) of the sample
4. **Seamless Loop Toggle**: Enables/disables seamless loop processing
5. **Pitch Slider**: Adjusts pitch in semitones (-12 to +12)
6. **Sample Rate Slider**: Multiplies sample rate (0.5x to 2.0x)
7. **Save Processed Sample Button**: Saves the current processed sample to disk

## Supported Formats

- **Input**: WAV, AIF, AIFF, FLAC
- **Output**: WAV (24-bit)

## Integration with JUCE Project

The SampleEditor files have been added to the Rain.jucer project file in the UI/SampleEditor group:

- SampleEditor.h (header file)
- SampleEditor.cpp (compiled source)
- Scripts.cs (namespace header)

## Differences from Scriptable Object

Unlike the previous scriptable object implementation that created temporary copies for preview:

1. **Permanent Saving**: All edits can be saved as permanent audio files
2. **Real-time Preview**: Changes are applied in real-time to the display
3. **Standalone Window**: Operates in its own editor window
4. **Enhanced Controls**: More intuitive slider-based interface
5. **Better Integration**: Designed to work seamlessly with the existing JUCE architecture

## Technical Details

- **Thread Safety**: Uses atomic pointers for safe buffer access across threads
- **Memory Management**: Efficient shared_ptr usage for audio buffers
- **Processing**: Real-time audio processing with immediate visual feedback
- **File I/O**: Robust audio file loading and saving using JUCE's AudioFormatManager

## Example Workflow

1. Open the Sample Editor window
2. Load an audio file (drag & drop or Load button)
3. Adjust start/end positions using sliders
4. Enable seamless loop if desired
5. Adjust pitch and sample rate as needed
6. Preview changes in real-time in the waveform display
7. Save the processed sample using the Save button
8. Use the saved sample in your project