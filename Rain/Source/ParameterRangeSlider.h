// ParameterRangeSlider.h ──────────────────────────────────────────────────
#pragma once
#include <JuceHeader.h>

class ParameterRangeSlider : public juce::Component
{
public:
    /**
        @param lowParamID   Parameter that drives the slider's *min* handle
        @param highParamID  Parameter that drives the slider's *max* handle
        @param style        Any TwoValue style (Horizontal/Vertical)
    */
    ParameterRangeSlider(juce::AudioProcessorValueTreeState& s,
        const juce::String& lowParamID,
        const juce::String& highParamID,
        juce::Slider::SliderStyle style = juce::Slider::TwoValueHorizontal,
        bool showTextBox = false,
        const juce::String& title = "")
        : apvts(s), lowID(lowParamID), highID(highParamID)
    {
        // ── Fetch and sanity-check both parameters ───────────────────────
        auto* low = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(lowID));
        auto* high = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(highID));
        jassert(low && high);                                           // same as you did

        const auto& r = low->range;  // assume identical ranges

        // ── Configure the slider from the range once ────────────────────
        slider.setSliderStyle(style);
        slider.setRange(r.start, r.end, r.interval);
        slider.setSkewFactor(r.skew);
        slider.setMinAndMaxValues(low->get(), high->get(), juce::dontSendNotification);
        slider.setTextValueSuffix(" " + low->getLabel());

        slider.setName(lowID + " -> " + highID);
        slider.setTitle(slider.getName());

        if (title != "")
            slider.setTitle(title);

        if (showTextBox)
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        else
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        addAndMakeVisible(slider);

        // ── Attach both parameters (bidirectional) ──────────────────────
        //    ParameterAttachment lets you define your own "write" lambda.
        lowAttach = std::make_unique<juce::ParameterAttachment>(
            *low, [this](float v) { setSliderMin(v); }, apvts.undoManager);

        highAttach = std::make_unique<juce::ParameterAttachment>(
            *high, [this](float v) { setSliderMax(v); }, apvts.undoManager);

        slider.onValueChange = [this, low, high]
            {
                if (ignoreCallbacks) return;         // avoid feedback loop

                const float newMin = (float)slider.getMinValue();
                const float newMax = (float)slider.getMaxValue();

                const auto& rng = low->range;

                juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);

                low->beginChangeGesture();
                low->setValueNotifyingHost(rng.convertTo0to1(newMin)); 
                low->endChangeGesture();

                high->beginChangeGesture();
                high->setValueNotifyingHost(rng.convertTo0to1(newMax));
                high->endChangeGesture();
            };

    }

    void resized() override { slider.setBounds(getLocalBounds()); }
    juce::Slider& getSlider() noexcept { return slider; }

private:
    // Called by the ParameterAttachment lambdas
    void setSliderMin(float v)
    {
        juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        slider.setMinValue(v, juce::dontSendNotification);
    }
    void setSliderMax(float v)
    {
        juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        slider.setMaxValue(v, juce::dontSendNotification);
    }

    juce::AudioProcessorValueTreeState& apvts;
    juce::String         lowID, highID;
    juce::Slider         slider{ juce::Slider::TwoValueHorizontal, juce::Slider::TextBoxBelow };
    std::unique_ptr<juce::ParameterAttachment> lowAttach, highAttach;
    bool ignoreCallbacks{ false };  // guards recursion
};
