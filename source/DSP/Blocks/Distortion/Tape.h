//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../../Utils/Utils.h"
#include "BaseDistortion.h"

namespace viator::dsp
{
    class Tape final : public BaseDistortion
    {
    public:
        Tape() = default;

        void prepare(const juce::dsp::ProcessSpec& spec) override
        {
            m_spec = spec;
            BaseDistortion::prepare(m_spec);

            m_compressor.prepare(m_spec);
            m_compressor.setRatio(2.0f);
            m_compressor.setRelease(60.0f);
            m_compressor.setAttack(120.0f);
            m_compressor.setThreshold(-0.1);

            m_low_shelf.prepare(m_spec);
        }


        void process(juce::dsp::AudioBlock<float>& block) override
        {
            for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
                auto *data = block.getChannelPointer(channel);
                for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                    const auto ch = static_cast<int>(channel);
                    const float drive = getDrives()[ch].getNextValue();
                    const float drive_comp = getDriveComps()[ch].getNextValue();
                    const float mix = getMixes()[ch].getNextValue();
                    const float xn = data[sample];

                    float yn = viator::dsp_utils::softClip(xn, drive);
                    yn = m_compressor.processSample(ch, yn * getInputComp());
                    yn *= getOutputComp() * drive_comp;

                    data[sample] = viator::dsp_utils::mixSamples(xn, yn, mix);
                }
            }

            updateFilter();
            m_low_shelf.process(juce::dsp::ProcessContextReplacing<float>(block));

            BaseDistortion::processBlock(block);
        }

        void setDrive(const float newValue) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newValue));
            }

            for (auto& drive : getDriveComps()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newValue * -0.65f));
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
        juce::dsp::Compressor<float> m_compressor;
        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> m_low_shelf;

        void updateFilter() const {
            *m_low_shelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                    m_spec.sampleRate,
                    100.0f, 1.5f, 1.5f
            );
        }
    };
}
