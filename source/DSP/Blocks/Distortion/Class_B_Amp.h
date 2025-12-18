//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include "BaseDistortion.h"

namespace viator::dsp
{
    class ClassBAmp final : public BaseDistortion
    {
    public:
        ClassBAmp() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            BaseDistortion::prepare(m_spec);

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


        void process(juce::dsp::AudioBlock<float>& block) override
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();
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

            BaseDistortion::processBlock(block);
        }

        void setDrive(const float newValue) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newValue));
            }
        }

        void setMix(const float newValue) override
        {
            for (auto& mix : getMixes()) {
                mix.setTargetValue(newValue * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec {};
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_positive_dc, m_negative_dc;
    };
}
