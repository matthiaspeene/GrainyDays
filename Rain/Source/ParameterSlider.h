// ParameterSlider.h ────────────────────────────────────────────────────────
#pragma once
#include <JuceHeader.h>

class ParameterSlider : public juce::Component
{
public:
    using Attachment = juce::SliderParameterAttachment;      // JUCE 7

    ParameterSlider(juce::AudioProcessorValueTreeState& s,
        const juce::String& paramID,
        juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag,
        bool showTextBox = true)
        : apvts(s), id(paramID)
    {
        // 1. Grab the parameter once, keep the pointer.
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(id)))
        {
            param = p;                                     // will never be nullptr in a release build
            const auto& r = param->range;

            // 2. Configure the slider from the parameter’s range.
            slider.setSliderStyle(style);
            slider.setRange(r.start, r.end, r.interval);
            slider.setSkewFactor(r.skew);                 // handles log / exp ranges
            slider.setTextValueSuffix(" " + param->getLabel());

            if (showTextBox)
                slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
            else
                slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

            slider.textFromValueFunction = [p](double v) { return p->getCurrentValueAsText(); };

            addAndMakeVisible(slider);

            // 4. Wire it up – SliderParameterAttachment automatically handles undo/redo
            attachment = std::make_unique<Attachment>(*param, slider, apvts.undoManager);
        }
        else
        {
            jassertfalse;   // bad ID?
        }
    }

    void resized() override { slider.setBounds(getLocalBounds()); }

    juce::Slider& getSlider() noexcept { return slider; }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::String                         id;
    juce::AudioParameterFloat* param{ nullptr };
    juce::Slider                         slider;
    std::unique_ptr<Attachment>          attachment;
};
