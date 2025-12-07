//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include "BaseDistortion.h"

namespace viator::dsp
{
    class Overdrive final : public BaseDistortion
    {
    public:
        Overdrive() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            BaseDistortion::prepare(m_spec);

            for (auto& filter : m_hp_filter) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(720.0f);
            }

            for (auto& filter : m_lp_filter) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
                filter.setCutoffFrequency(3500.0f);
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
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);

                for (size_t sample = 0; sample < block.getNumSamples(); ++sample)
                {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float drive_comp = getDriveComps()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();

                    const float xn = data[sample] * getInputComp();

                    float yn = m_hp_filter[ch].processSample(ch, xn);

                    float yn_neg = viator::dsp_utils::polettiWaveshaper(yn, drive, 0.6f, 6.6f);
                    float yn_pos = viator::dsp_utils::polettiWaveshaper(yn, drive, 6.6f, 0.6f);

                    yn_pos = m_positive_dc[ch].processSample(ch, yn_pos);
                    yn_neg = m_negative_dc[ch].processSample(ch, yn_neg);

                    yn = m_lp_filter[ch].processSample(ch, yn_pos + yn_neg);

                    yn *= getOutputComp() * drive_comp;

                    data[sample] = viator::dsp_utils::mixSamples(xn, yn, mix);
                }
            }

            BaseDistortion::processBlock(block);
        }

        void setDrive(float newDrive) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
            }

            for (auto& drive : getDriveComps()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive * -0.5f));
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
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_hp_filter, m_lp_filter, m_positive_dc, m_negative_dc;
    };
}
