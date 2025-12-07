//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"

namespace viator::dsp
{
    class Tape
    {
    public:
        Tape() = default;

        enum Channel
        {
            kLeft = 0,
            kRight,
            num_channels
        };

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            m_spec = spec;

            for (auto& drive : m_drive_smoothers) {
                drive.reset(spec.sampleRate, 0.02);
            }

            for (auto& drive : m_drive_comp_smoothers) {
                drive.reset(spec.sampleRate, 0.02);
            }

            for (auto& mix : m_mix_smoothers) {
                mix.reset(spec.sampleRate, 0.02);
            }

            m_compressor.prepare(m_spec);
            m_compressor.setRatio(4.0f);
            m_compressor.setRelease(60.0f);
            m_compressor.setAttack(30.0f);
        }


        void process(juce::dsp::AudioBlock<float>& block)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = m_drive_smoothers[ch].getNextValue();
                    const float drive_comp = m_drive_comp_smoothers[ch].getNextValue();
                    const float mix = m_mix_smoothers[ch].getNextValue();
                    const float xn = data[sample];
                    float yn = viator::dsp_utils::softClip(xn, drive);
                    yn = m_compressor.processSample(ch, yn * m_input_comp);
                    yn *= m_output_comp * drive_comp;
                    data[sample] = viator::dsp_utils::mixSamples(xn, yn, mix);
                }
            }
        }

        void setDrive(float newDrive)
        {
            for (auto& drive : m_drive_smoothers) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
            }

            for (auto& drive : m_drive_comp_smoothers) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive * -0.65f));
            }

            m_compressor.setThreshold(newDrive * -0.1f);
        }

        void setMix(const float newMix)
        {
            for (auto& mix : m_mix_smoothers) {
                mix.setTargetValue(newMix * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec{};
        std::array<juce::SmoothedValue<float>, num_channels> m_drive_smoothers, m_mix_smoothers, m_drive_comp_smoothers;
        juce::dsp::Compressor<float> m_compressor;
        const float m_input_comp = juce::Decibels::decibelsToGain(18.0f);
        const float m_output_comp = juce::Decibels::decibelsToGain(-18.0f);
    };
}
