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
            m_compressor.setRatio(4.0f);
            m_compressor.setRelease(60.0f);
            m_compressor.setAttack(30.0f);
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

            BaseDistortion::processBlock(block);
        }

        void setDrive(float newDrive) override
        {
            for (auto& drive : getDrives()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
            }

            for (auto& drive : getDriveComps()) {
                drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive * -0.65f));
            }

            m_compressor.setThreshold(newDrive * -0.1f);
        }

        void setMix(const float newMix) override
        {
            for (auto& mix : getMixes()) {
                mix.setTargetValue(newMix * 0.01f);
            }
        }

    private:
        juce::dsp::ProcessSpec m_spec {};
        juce::dsp::Compressor<float> m_compressor;
    };
}
