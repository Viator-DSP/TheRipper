//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include "BaseDistortion.h"

namespace viator::dsp
{
    class BitReducer final : public BaseDistortion
    {
    public:
        BitReducer() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            const auto numChannels = static_cast<size_t>(m_spec.numChannels);
            downSampleCounters.assign(numChannels, 0);
            lastQuantizedSamples.assign(numChannels, 0.0f);
            BaseDistortion::prepare(m_spec);
        }

        void process(juce::dsp::AudioBlock<float>& block) override {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float mix = getMixes()[ch].getNextValue();
                    const float drive = getDrives()[ch].getNextValue();
                    float xn = data[sample];

                    float m_amp_values = std::pow(2.0f, bitDepthLowerLimit);
                    float yn = xn * m_amp_values;
                    yn = std::round(yn);
                    yn /= m_amp_values;

                    yn = viator::dsp_utils::hardClip(yn, drive);

                    data[sample] = viator::dsp_utils::mixSamples(data[sample], yn, mix);
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

        void setBitDepth(const float newValue)
        {
            const auto safeDepth = static_cast<int>(juce::jlimit(bitDepthLowerLimit, bitDepthUpperLimit, newValue));
            bitDepth = static_cast<float>(safeDepth);
        }

    private:
        juce::dsp::ProcessSpec m_spec {};
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_filters;

        float bitDepth { 4.0f };
        static constexpr float bitDepthLowerLimit { 4.0f };
        static constexpr float bitDepthUpperLimit { 16.0f };
        const float m_amp_values = std::pow(2.0f, 16.0f);

        std::vector<int> downSampleCounters;
        std::vector<float> lastQuantizedSamples;
    };
}
