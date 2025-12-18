//
// Created by Landon Viator on 10/15/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


namespace viator::gui_utils
{
    class Images final
    {
    public:

        Images() {
            m_bit_icon = juce::Drawable::createFromImageData(BinaryData::fadmodrandom_svg,
                                                                 BinaryData::fadmodrandom_svgSize);
        }

        static const juce::Image &logo() { return images().m_logo; }
        static const juce::Image &texture() { return images().m_texture; }
        static const juce::Image &meter() { return images().m_meter; }
        static const juce::Image &basic_knob() { return images().m_basic_knob; }
        static const juce::Image &chicken_head_knob() { return images().m_chicken_head_knob; }
        static const juce::Image &synth_knob() { return images().m_synth_knob; }

        static const std::unique_ptr<juce::Drawable>& bit_icon()
        {
            return images().m_bit_icon;
        }

    private:

        static Images &images()
        {
            static Images instance;
            return instance;
        }

    protected:
        juce::Image m_logo{juce::ImageCache::getFromMemory(BinaryData::landon_png,
        BinaryData::landon_pngSize)};
        juce::Image m_texture{juce::ImageCache::getFromMemory(BinaryData::lightgrayconcretewall_jpg,
        BinaryData::lightgrayconcretewall_jpgSize)};
        juce::Image m_meter{juce::ImageCache::getFromMemory(BinaryData::neptune_vu_png,
                                                              BinaryData::neptune_vu_pngSize)};
        juce::Image m_basic_knob{juce::ImageCache::getFromMemory(BinaryData::Knob_03_png,
                                                              BinaryData::Knob_03_pngSize)};
        juce::Image m_chicken_head_knob{juce::ImageCache::getFromMemory(BinaryData::Knob_01_png,
                                                      BinaryData::Knob_01_pngSize)};
        juce::Image m_synth_knob{juce::ImageCache::getFromMemory(BinaryData::Knob_05_png,
                                                      BinaryData::Knob_05_pngSize)};
        std::unique_ptr<juce::Drawable> m_bit_icon;

        JUCE_DECLARE_NON_COPYABLE(Images)
    };
}