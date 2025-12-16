//
// Created by Landon Viator on 12/12/25.
//

#pragma once
#include "../Utils/FilmStripImage.h"
#include "GUI/Utils/Images.h"

namespace viator::gui::widgets {
    class KnobUtils {
    public:
        static void draw_ticks(juce::Graphics &g, const juce::Slider &slider, const float radiusMult = 0.05f) {
            const auto bounds = slider.getLocalBounds().toFloat().reduced(static_cast<float>(slider.getWidth()) * 0.1f);
            const auto center = bounds.getCentre();
            const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 10.0f;

            constexpr int numSteps = 11;
            constexpr float startAngle = juce::MathConstants<float>::pi * 1.25f; // 225°
            constexpr float endAngle = juce::MathConstants<float>::pi * 2.75f; // 495°

            g.setColour(slider.findColour(juce::Slider::ColourIds::trackColourId));
            const auto font_size = static_cast<float>(slider.getHeight()) * 0.12f;
            const auto font = viator::gui_utils::Fonts::bold(juce::jmin(font_size, 12.0f));
            g.setFont(font);

            for (int i = 0; i < numSteps; ++i) {
                const float t = static_cast<float>(i) / (numSteps - 1);
                const auto value = static_cast<float>(slider.getNormalisableRange().convertFrom0to1(t));
                const auto angle = juce::jmap(t, 0.0f, 1.0f, startAngle, endAngle) - juce::MathConstants<float>::halfPi;

                const float labelRadius = radius + static_cast<float>(slider.getHeight()) * 0.1f;
                const float x = center.x + std::cos(angle) * labelRadius;
                const float y = center.y + std::sin(angle) * labelRadius;

                juce::String label;
                if (std::abs(value) >= 1000.0f) {
                    const int valueInK = static_cast<int>(std::round(value / 1000.0f));
                    label = juce::String(valueInK) + "k";
                } else {
                    label = juce::String(static_cast<int>(std::round(std::abs(value))));
                }

                const float textWidth = g.getCurrentFont().getStringWidth(label);
                const float textHeight = g.getCurrentFont().getAscent();

                if (i == 0 || i == 5 || i == 10) {
                    juce::Rectangle<float> textBounds(x - textWidth / 2.0f, y - textHeight / 2.0f, textWidth,
                                                      textHeight);
                    g.drawText(label, textBounds, juce::Justification::centred);
                }
            }

            g.setColour(juce::Colours::white);
        }
    };

    class BaseKnob : public juce::Slider {
    public:
        BaseKnob() {
            setSliderStyle(juce::Slider::RotaryVerticalDrag);
            setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        }

        void paint(juce::Graphics &g) override {
            // slider
            const auto position = static_cast<float>(getNormalisableRange().convertTo0to1(getValue()));
            auto bounds = getLocalBounds().toFloat();
            constexpr auto mult = 0.95;
            const auto y = juce::roundToInt(getHeight() * 0.05);

            switch (m_knob_type) {
                case KnobType::kBasic: {
                    basic_knob.drawWithinCentered(g, juce::roundToInt(getWidth() * mult),
                                                  juce::roundToInt(getHeight() * mult),
                                                  position, bounds, 0, y);
                }
                break;
                case KnobType::kChicken: {
                    chicken_knob.drawWithinCentered(g, juce::roundToInt(getWidth() * mult),
                                                  juce::roundToInt(getHeight() * mult),
                                                  position, bounds, 0, y);
                }
                    break;
                case KnobType::kSynth: {
                    synth_knob.drawWithinCentered(g, juce::roundToInt(getWidth() * mult),
                                                  juce::roundToInt(getHeight() * mult),
                                                  position, bounds, 0, y);
                }
                    break;
            }

            KnobUtils::draw_ticks(g, *this);

            // g.setColour(juce::Colours::white);
            // g.drawRect(getLocalBounds());
        }

        enum class KnobType {
            kBasic,
            kChicken,
            kSynth
        };

        void setKnobType(KnobType knobType) {
            m_knob_type = knobType;
            repaint();
        }

    private:
        FilmStripImage basic_knob{viator::gui_utils::Images::basic_knob(), 129};
        FilmStripImage chicken_knob{viator::gui_utils::Images::chicken_head_knob(), 257};
        FilmStripImage synth_knob{viator::gui_utils::Images::synth_knob(), 129};

        KnobType m_knob_type = KnobType::kBasic;
    };
}
