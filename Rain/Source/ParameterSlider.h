#pragma once
#include <JuceHeader.h>

class ParameterSlider : public juce::Component
{
public:
    // convenient aliases
    using Attachment = juce::SliderParameterAttachment;   // single value
    using GenericAttachment = juce::ParameterAttachment;         // two values

    // ──────────────────────────────────────────────── Single value constructor
    ParameterSlider(juce::AudioProcessorValueTreeState& s,
        const juce::String& paramID,
        juce::Slider::SliderStyle           style = juce::Slider::RotaryHorizontalVerticalDrag,
        bool                                showTextBox = true,
        juce::String                        nameIn = {}) :
        apvts(s), name(nameIn), showTextBox(showTextBox)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(paramID)))
        {
            paramLo = p;
            configureSlider(style, p->range, showTextBox, p->getLabel());
            attachSingle(*p);
        }
        else
            jassertfalse;    // bad ID?
    }

    // ──────────────────────────────────────────────── Two-value constructor
    ParameterSlider(juce::AudioProcessorValueTreeState& s,
        const juce::String& lowParamID,
        const juce::String& highParamID,
        juce::Slider::SliderStyle           style = juce::Slider::TwoValueHorizontal,
        bool                                showTextBox = false,
        juce::String                        nameIn = {}) :
        apvts(s), name(nameIn), showTextBox(showTextBox)
    {
        paramLo = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(lowParamID));
        paramHi = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter(highParamID));

        if (paramLo != nullptr && paramHi != nullptr)
        {
            // both parameters share an identical range
            configureSlider(style, paramLo->range, false, paramLo->getLabel());          // main range slider
            createTextBoxes(paramLo->range, paramLo->getLabel());                        // editable min / max boxes

            // main slider ─► parameters
            slider.onValueChange = [this]
                {
                    if (paramLo && paramHi)
                    {
                        paramLo->setValueNotifyingHost(paramLo->convertTo0to1((float)slider.getMinValue()));
                        paramHi->setValueNotifyingHost(paramHi->convertTo0to1((float)slider.getMaxValue()));
                    }
                };

            // parameters ─► main slider
            loAttach = std::make_unique<GenericAttachment>(
                *paramLo,
                [this](float newNorm)
                {
                    if (!slider.isMouseButtonDown())
                        slider.setMinValue(newNorm, juce::sendNotificationSync, true);
                },
                apvts.undoManager);

            hiAttach = std::make_unique<GenericAttachment>(
                *paramHi,
                [this](float newNorm)
                {
                    if (!slider.isMouseButtonDown())
                        slider.setMaxValue(newNorm, juce::sendNotificationSync, true);
                },
                apvts.undoManager);
        }
        else
            jassertfalse;    // bad IDs?
    }

    // ──────────────────────────────────────────────── Component overrides
    void resized() override
    {
        auto area = getLocalBounds();

        // leave space for the range-label or rotary title
        if (name.isNotEmpty())
        {
            if (slider.getSliderStyle() == juce::Slider::TwoValueHorizontal)
                area.removeFromLeft(45);
            else
                area.removeFromTop(20);
        }

        // reserve space for the editable boxes
        juce::Rectangle<int> boxArea;
        if (showTextBox && slider.getSliderStyle() == juce::Slider::TwoValueHorizontal)
            boxArea = area.removeFromRight(58);

        slider.setBounds(area);

        // lay out the two text boxes (stacked vertically)
        if (showTextBox && paramLo && paramHi)
        {
            auto top = boxArea.removeFromTop(boxArea.getHeight() / 2);
            minBox.setBounds(top.reduced(1));
            maxBox.setBounds(boxArea.reduced(1));
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.setFont(16.0f);
        g.setColour(juce::Colours::black);

        // draw the label (if any)
        if (name.isNotEmpty())
        {
            if (slider.getSliderStyle() == juce::Slider::TwoValueHorizontal)
                g.drawText(name, getLocalBounds().removeFromLeft(45), juce::Justification::centredLeft);
            else
                g.drawText(name, getLocalBounds().removeFromTop(20), juce::Justification::centred);
        }
    }

    juce::Slider& getSlider() noexcept { return slider; }

private:
    // ──────────────────────────────────────────────── helpers
    void configureSlider(juce::Slider::SliderStyle          st,
        const juce::NormalisableRange<float>& r,
        bool                               wantTextBox,
        const juce::String suffix)
    {
        slider.setSliderStyle(st);
        slider.setRange(r.start, r.end, r.interval);
        slider.setSkewFactor(r.skew);
        slider.setTextValueSuffix(suffix);

        if (wantTextBox)
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 20);
        else
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        addAndMakeVisible(slider);
    }

    void attachSingle(juce::AudioParameterFloat& p)
    {
        slider.setTextValueSuffix(" " + p.getLabel());
        slider.textFromValueFunction = [&p](double) { return p.getCurrentValueAsText(); };
        singleAttach = std::make_unique<Attachment>(p, slider, apvts.undoManager);
    }

    // build editable min / max boxes and hook them up
    void createTextBoxes(const juce::NormalisableRange<float>& r, const juce::String suffix)
    {
        auto setup = [&](juce::Slider& box)
            {
                box.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
				box.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::transparentBlack);
                box.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 58, 20); // only the text field is visible
				box.setTextValueSuffix(" " + suffix);
                box.setRange(r.start, r.end, r.interval);
                box.setSkewFactor(r.skew);
                box.setVelocityModeParameters(0.01, 1, 0.0, false);
                // hide the value-popup completely
                box.setPopupDisplayEnabled(false, false, nullptr);
                addAndMakeVisible(box);
            };

        setup(minBox);
        setup(maxBox);

        minAttach = std::make_unique<Attachment>(*paramLo, minBox, apvts.undoManager);
        maxAttach = std::make_unique<Attachment>(*paramHi, maxBox, apvts.undoManager);
    }

    // ──────────────────────────────────────────────── members
    juce::AudioProcessorValueTreeState& apvts;
    juce::String                         name;

    juce::Slider                         slider;   // rotary or two-value range
    juce::Slider                         minBox, maxBox; // editable numeric fields

    juce::AudioParameterFloat* paramLo{ nullptr };
    juce::AudioParameterFloat* paramHi{ nullptr };

    // attachments
    std::unique_ptr<Attachment>          singleAttach; // single value
    std::unique_ptr<GenericAttachment>   loAttach, hiAttach;    // range -> slider
    std::unique_ptr<Attachment>          minAttach, maxAttach;   // param  -> text boxes

    bool                                 showTextBox{ false };
};
