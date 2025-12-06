//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace viator::dsp
{
    class ClassBAmp
    {
    public:
        ClassBAmp()
        {

        }

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

            for (auto& filter : m_negative_dc) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(5.0f);
            }

            for (auto& filter : m_positive_dc) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(5.0f);
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
                    const auto reduced_input = xn * drive * 0.05f;

                    float positive = viator::dsp_utils::polettiWaveshaper(reduced_input, 1.5f, 0.6f, 6.6f);
                    float negative = viator::dsp_utils::polettiWaveshaper(reduced_input, 1.5f, 6.6f, 0.6f);

                    positive = m_positive_dc[ch].processSample(ch, positive);
                    negative = m_negative_dc[ch].processSample(ch, negative);

                    constexpr auto coeff = 0.4f;
                    positive = viator::dsp_utils::polettiWaveshaper(positive, 3.0f, coeff, coeff);
                    negative = viator::dsp_utils::polettiWaveshaper(negative, 3.0f, coeff, coeff);
                    const float blend = (positive + negative) * 2.6f;

                    data[sample] = viator::dsp_utils::mixSamples(xn, blend, mix);
                }
            }
        }

        void setDrive(float newDrive)
        {
            for (auto& drive : m_drive_smoothers) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
            }
        }

        void setMix(const float newMix)
        {
            for (auto& mix : m_mix_smoothers) {
                mix.setTargetValue(newMix * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec;
        std::array<juce::SmoothedValue<float>, num_channels> m_drive_smoothers, m_mix_smoothers;
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_positive_dc, m_negative_dc;
    };
}
