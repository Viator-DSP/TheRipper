//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"

namespace viator::dsp
{
    class CircleMap
    {
    public:
        CircleMap() = default;

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

            for (auto& mix : m_mix_smoothers) {
                mix.reset(spec.sampleRate, 0.02);
            }
        }


        void process(juce::dsp::AudioBlock<float>& block)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = m_drive_smoothers[ch].getNextValue();
                    const float mix = m_mix_smoothers[ch].getNextValue();
                    const float xn = data[sample];
                    const float yn = viator::dsp_utils::circleMapWaveshaper(xn, drive);
                    data[sample] = viator::dsp_utils::mixSamples(xn, yn, mix);
                }
            }
        }

        void setDrive(float newDrive)
        {
            for (auto& drive : m_drive_smoothers) {
                drive.setTargetValue(newDrive * 0.033f);
            }
        }

        void setMix(const float newMix)
        {
            for (auto& mix : m_mix_smoothers) {
                mix.setTargetValue(newMix * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec{};
        std::array<juce::SmoothedValue<float>, num_channels> m_drive_smoothers, m_mix_smoothers;
    };
}
