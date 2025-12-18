//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include "BaseDistortion.h"

namespace viator::dsp
{
    class CircleMap final : public BaseDistortion
    {
    public:
        CircleMap() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            BaseDistortion::prepare(m_spec);
            for (auto &drive: m_drive_smoothers)
            {
                drive.reset(spec.sampleRate, 0.02);
            }
        }

        void process(juce::dsp::AudioBlock<float>& block) override
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float drive_extra = m_drive_smoothers[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();
                    const float xn = data[sample];
                    float yn = xn;
                    yn = viator::dsp_utils::arraya(yn * drive_extra);
                    yn = viator::dsp_utils::circleMapWaveshaper(yn, drive);
                    data[sample] = viator::dsp_utils::mixSamples(data[sample], yn, mix);
                }
            }

            BaseDistortion::processBlock(block);
        }

        void setDrive(const float newValue) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(newValue * 0.033f);
            }

            for (auto &drive: m_drive_smoothers)
            {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newValue * 0.15f));
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
        std::array<juce::SmoothedValue<float>, num_channels> m_drive_smoothers;
    };
}
