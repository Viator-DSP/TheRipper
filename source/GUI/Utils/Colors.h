//
// Created by Landon Viator on 11/24/25.
//

#pragma once

namespace viator::gui_utils
{
    class Colors final
    {
    public:

        static juce::Colour theme()      { return {52, 76, 100}; }
        static juce::Colour editor_bg_color()      { return {15, 15, 15}; }
        static juce::Colour info_bg_color()      { return {67, 72, 80}; }
        static juce::Colour header_bg_color()      { return {57, 62, 70}; }
        static juce::Colour widget_off()      { return {186, 187, 188}; }
        static juce::Colour widget_on()      { return {110, 172, 218}; }
        static juce::Colour widget_center()      { return {39, 39, 39}; }
        static juce::Colour widget_dark()      { return {29, 29, 29}; }
        static juce::Colour power_on()      { return {120, 185, 181}; }
        static juce::Colour text()      { return {249, 250, 251}; }
        static juce::Colour meter_green()      { return {59, 151, 151}; }
        static juce::Colour meter_yellow()      { return {255, 204, 0}; }
        static juce::Colour meter_red()      { return {247, 55, 79}; }
    };
}
