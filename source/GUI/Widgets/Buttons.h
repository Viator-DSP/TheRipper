//
// Created by Landon Viator on 12/19/25.
//

#pragma once
#include "GUI/Utils/Images.h"
#include "GUI/Utils/Fonts.h"

namespace viator
{
    class ImageButton : public juce::ImageButton {
    public:
        ImageButton()
        {
            const auto off = viator::gui_utils::Images::button_off();
            const auto on = viator::gui_utils::Images::button_on();
            setImages(false, true, true, off,
                      1.0f, juce::Colours::transparentBlack, off, 1.0f,
                      juce::Colours::white.withAlpha(0.1f), on, 1.0f,
                      juce::Colours::transparentBlack);
            setClickingTogglesState(true);
        }

        void paint(juce::Graphics &g) override
        {
            juce::ImageButton::paint(g);

            const auto text = getButtonText();
            const auto font_size = getHeight() / 4;
            const auto font = viator::gui_utils::Fonts::regular(static_cast<float>(font_size));
            const auto text_y = juce::roundToInt(getHeight() * -0.06);
            g.setFont(font);
            g.setColour(juce::Colours::white);
            g.drawText(text, getLocalBounds().withY(text_y), juce::Justification::centred);
        }
    };
}
