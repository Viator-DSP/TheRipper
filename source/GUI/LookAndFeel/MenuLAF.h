//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils/Colors.h"
#include "../Utils/Fonts.h"

namespace viator::gui::laf
{
    class MenuLAF final : public juce::LookAndFeel_V4
    {
    public:

        void drawComboBox (juce::Graphics& g, int width, int height, bool,
                                   int, int, int, int, juce::ComboBox& box) override
        {
            auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 6.0f;
            juce::Rectangle<int> boxBounds (0, 0, width, height);

            g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
            g.fillRoundedRectangle (boxBounds.toFloat().reduced(3.0f, 3.0f), cornerSize);

            juce::Rectangle<int> arrowZone (width - 30, 0, 20, height);
            juce::Path path;
            path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
            path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
            path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

            g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
            g.strokePath (path, juce::PathStrokeType (2.0f));
        }

        juce::Font getComboBoxFont(juce::ComboBox &box) override
        {
            const auto font_size = static_cast<float>(box.getHeight()) * 0.5f;
            return viator::gui_utils::Fonts::regular(font_size);
        }
    };
}
