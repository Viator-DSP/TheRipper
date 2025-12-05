#pragma once

#include "PluginProcessor.h"
#include "GUI/Components/Header.h"
#include "GUI/Components/InfoPanel.h"
#include "GUI/LookAndFeel/ThinDialLAF.h"
#include "GUI/Components/LevelMeter.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::ActionListener,
public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);

    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;

    void resized() override;

    enum IO_Sliders
    {
        kLeft = 0,
        kRight,
        num_io_sliders
    };

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor &processorRef;

    viator::gui::components::Header m_header;
    viator::gui::components::InfoPanel m_info_panel;

    void actionListenerCallback(const juce::String &message) override;
    void apply_preset(const bool isPresetA) const ;
    void save_preset(const juce::String &param, const float paramValue);

    std::array<juce::Slider, num_io_sliders> m_sliders;
    std::array<juce::Label, num_io_sliders> m_slider_popup_labels;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_gain_attach, m_volume_attach;
    viator::gui::laf::DialLAF m_thin_dial_laf;
    void initSliders();

    std::array<viator::gui::LevelMeter, num_io_sliders> m_input_meter, m_output_meter;

    void timerCallback() override;
    void initMeters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
