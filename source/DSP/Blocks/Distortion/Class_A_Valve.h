//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include "BaseDistortion.h"

namespace viator::dsp
{
    class Class_A_Valve final : public BaseDistortion
    {
    public:
        Class_A_Valve() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            BaseDistortion::prepare(m_spec);

            for (auto& filter : m_dc_filter) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(20.0f);
            }

            for (auto& filter : m_band_filter) {
                filter.prepare(m_spec);
                filter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
                filter.setCutoffFrequency(100.0f);
            }

            m_lp_filter.prepare(m_spec);
            m_lp_filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
            m_lp_filter.setCutoffFrequency(10000.0f);

            m_low_shelf.prepare(spec);

            // initial coefficients
            *m_low_shelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                    m_spec.sampleRate,
                    100.0f,   // freq
                    0.707f,   // Q or slope
                    juce::Decibels::decibelsToGain(3.0f)
            );
        }


        void process(juce::dsp::AudioBlock<float>& block) override
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();

                    float band_low, band_high;
                    m_band_filter[ch].processSample(ch, data[sample], band_low, band_high);

                    band_high = viator::dsp_utils::valveGridConduction(band_high * drive, 1.5f);
                    float yn = viator::dsp_utils::classAValve(band_high, drive, 1.5f, 4.0f, -1.5f);
                    yn = m_dc_filter[ch].processSample(ch, yn);
                    data[sample] = viator::dsp_utils::mixSamples(data[sample], band_low + yn, mix);
                }
            }

            m_low_shelf.process(juce::dsp::ProcessContextReplacing<float>(block));
            m_lp_filter.process(juce::dsp::ProcessContextReplacing<float>(block));

            BaseDistortion::processBlock(block);
        }

        void setDrive(float newDrive) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
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
        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_channels> m_dc_filter, m_band_filter;
        juce::dsp::LinkwitzRileyFilter<float> m_lp_filter;

        juce::dsp::ProcessorDuplicator<
                juce::dsp::IIR::Filter<float>,
                juce::dsp::IIR::Coefficients<float>> m_low_shelf;
    };
}
