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
                filter.setCutoffFrequency(300.0f);
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

            m_tone.prepare(m_spec);

            getTiltEQ().setTiltPivot(400.0f);
        }

        void process(juce::dsp::AudioBlock<float>& block) override
        {
            updateToneFilter();
            m_tone.process(juce::dsp::ProcessContextReplacing<float>(block));

            BaseDistortion::processBlock(block);

            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
            {
                auto *data = block.getChannelPointer(channel);

                for (size_t sample = 0; sample < block.getNumSamples(); ++sample)
                {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();

                    const float xn = data[sample] * getInputComp();

                    float yn = m_hp_filter[ch].processSample(ch, xn);

                    float yn_neg = viator::dsp_utils::polettiWaveshaper(yn, drive, 0.6f, 6.6f);
                    float yn_pos = viator::dsp_utils::polettiWaveshaper(yn, drive, 6.6f, 0.6f);

                    yn_pos = m_positive_dc[ch].processSample(ch, yn_pos);
                    yn_neg = m_negative_dc[ch].processSample(ch, yn_neg);

                    yn = m_lp_filter[ch].processSample(ch, yn_pos + yn_neg);

                    data[sample] = viator::dsp_utils::mixSamples(data[sample], yn, mix);
                }
            }
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

        void setPeakDb(const float newValue)
        {
            m_tone_db = newValue * 3.0f;
        }

    private:
        juce::dsp::ProcessSpec m_spec {};
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_hp_filter, m_lp_filter, m_positive_dc, m_negative_dc;
        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> m_tone;
        float m_tone_db { 0.0f };

        void updateToneFilter() const {
            *m_tone.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                    m_spec.sampleRate,
                    800.0f,
                    0.707,
                    juce::Decibels::decibelsToGain(m_tone_db)
            );
        }
    };
}
