#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

namespace viator::gui_utils
{
    class Images final
    {
    public:
        Images()
        {
             m_bit_icon = juce::Drawable::createFromImageData(BinaryData::fadmodrandom_svg,
                                                              BinaryData::fadmodrandom_svgSize);
             m_tape_icon = juce::Drawable::createFromImageData(BinaryData::tape_svg,
                                                               BinaryData::tape_svgSize);
             m_overdrive_icon = juce::Drawable::createFromImageData(BinaryData::overdrive_svg,
                                                                    BinaryData::overdrive_svgSize);
             m_circle_icon = juce::Drawable::createFromImageData(BinaryData::circle_svg,
                                                                 BinaryData::circle_svgSize);
             m_tube_icon = juce::Drawable::createFromImageData(BinaryData::tube_svg,
                                                               BinaryData::tube_svgSize);
             m_amp_icon = juce::Drawable::createFromImageData(BinaryData::amp_svg,
                                                              BinaryData::amp_svgSize);

             recolourInPlace(m_bit_icon, { { juce::Colours::black, juce::Colours::white } });
             recolourInPlace(m_tape_icon, { { juce::Colours::black, juce::Colours::white } });
             recolourInPlace(m_overdrive_icon, { { juce::Colours::black, juce::Colours::white } });
             recolourInPlace(m_circle_icon, { { juce::Colours::black, juce::Colours::white } });
             recolourInPlace(m_amp_icon, { { juce::Colours::black, juce::Colours::white } });

             recolourInPlace(m_tube_icon, {
                 { juce::Colours::white, juce::Colours::transparentBlack },
                 { juce::Colours::black, juce::Colours::whitesmoke }
             });
        }

        ~Images()
        = default;

        const juce::Image& logo() { return m_logo; }
        const juce::Image& texture() { return m_texture; }
        const juce::Image& meter() { return m_meter; }
        const juce::Image& basic_knob() { return m_basic_knob; }
        const juce::Image& chicken_head_knob() { return m_chicken_head_knob; }
        const juce::Image& synth_knob() { return m_synth_knob; }
        const juce::Image& screw() { return m_screw; }
        const juce::Image& grill() { return m_grill; }
        const juce::Image& button_on() { return m_button_on; }
        const juce::Image& button_off() { return m_button_off; }

         const juce::Drawable* bit_icon() { return m_bit_icon.get(); }
         const juce::Drawable* tape_icon() { return m_tape_icon.get(); }
         const juce::Drawable* overdrive_icon() { return m_overdrive_icon.get(); }
         const juce::Drawable* circle_icon() { return m_circle_icon.get(); }
         const juce::Drawable* tube_icon() { return m_tube_icon.get(); }
         const juce::Drawable* amp_icon() { return m_amp_icon.get(); }

    private:

        using Repl = std::initializer_list<std::pair<juce::Colour, juce::Colour>>;

        void recolourInPlace(std::unique_ptr<juce::Drawable>& d, Repl repl)
        {
            if (!d) return;
            for (const auto& p : repl)
                d->replaceColour(p.first, p.second);
        }

    private:
        juce::Image m_logo{ juce::ImageCache::getFromMemory(BinaryData::landon_png, BinaryData::landon_pngSize) };
        juce::Image m_texture{ juce::ImageCache::getFromMemory(BinaryData::lightgrayconcretewall_jpg, BinaryData::lightgrayconcretewall_jpgSize) };
        juce::Image m_meter{ juce::ImageCache::getFromMemory(BinaryData::neptune_vu_png, BinaryData::neptune_vu_pngSize) };
        juce::Image m_basic_knob{ juce::ImageCache::getFromMemory(BinaryData::Knob_03_png, BinaryData::Knob_03_pngSize) };
        juce::Image m_chicken_head_knob{ juce::ImageCache::getFromMemory(BinaryData::Knob_01_png, BinaryData::Knob_01_pngSize) };
        juce::Image m_synth_knob{ juce::ImageCache::getFromMemory(BinaryData::Knob_05_png, BinaryData::Knob_05_pngSize) };
        juce::Image m_screw{ juce::ImageCache::getFromMemory(BinaryData::screw_png, BinaryData::screw_pngSize) };
        juce::Image m_grill{ juce::ImageCache::getFromMemory(BinaryData::decor_grid_png, BinaryData::decor_grid_pngSize) };
        juce::Image m_button_on{ juce::ImageCache::getFromMemory(BinaryData::button_big_on_png, BinaryData::button_big_on_pngSize) };
        juce::Image m_button_off{ juce::ImageCache::getFromMemory(BinaryData::button_big_off_png, BinaryData::button_big_off_pngSize) };

         std::unique_ptr<juce::Drawable> m_bit_icon;
         std::unique_ptr<juce::Drawable> m_tape_icon;
         std::unique_ptr<juce::Drawable> m_overdrive_icon;
         std::unique_ptr<juce::Drawable> m_circle_icon;
         std::unique_ptr<juce::Drawable> m_tube_icon;
         std::unique_ptr<juce::Drawable> m_amp_icon;

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}