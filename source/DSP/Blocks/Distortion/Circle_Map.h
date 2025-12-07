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
        }

        void process(juce::dsp::AudioBlock<float>& block)
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();
                    const float xn = data[sample] * viator::dsp_utils::input_comp;
                    const float yn = viator::dsp_utils::circleMapWaveshaper(xn, drive) * viator::dsp_utils::output_comp;
                    data[sample] = viator::dsp_utils::mixSamples(xn, yn, mix);
                }
            }

            BaseDistortion::processBlock(block);
        }

        void setDrive(float newDrive) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(newDrive * 0.033f);
            }
        }

        void setMix(const float newMix) override
        {
            for (auto& mix : getMixes()) {
                mix.setTargetValue(newMix * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec {};
    };
}
