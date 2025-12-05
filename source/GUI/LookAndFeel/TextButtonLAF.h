//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils/Fonts.h"

namespace viator::gui::laf
{
    class TextButtonLAF final : public juce::LookAndFeel_V4
    {
    public:

        void drawButtonBackground (juce::Graphics& g,
                                           juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           const bool shouldDrawButtonAsHighlighted,
                                           const bool shouldDrawButtonAsDown) override
        {
            constexpr auto cornerSize = 6.0f;
            const auto bounds = button.getLocalBounds().toFloat().reduced (3.0f, 3.0f);

            auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                              .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

            g.setColour (baseColour);

            g.fillRoundedRectangle (bounds, cornerSize);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle (bounds, cornerSize, 2.0f);
        }

        juce::Font getTextButtonFont (juce::TextButton&, const int buttonHeight) override
        {
            const auto font_size = static_cast<float>(buttonHeight) * 0.5f;
            return viator::gui_utils::Fonts::regular(font_size);
        }
    };
}