//
// Created by Landon Viator on 12/7/25.
//

#pragma once
#include "../Filters/TiltEQ.h"

namespace viator::dsp
{
    class BaseDistortion
    {
    public:
        BaseDistortion() = default;
        virtual ~BaseDistortion() = default;

        enum Channel
        {
            kLeft = 0,
            kRight,
            num_channels
        };

        virtual void prepare(const juce::dsp::ProcessSpec& spec)
        {
            for (auto &drive: getDrives())
            {
                drive.reset(spec.sampleRate, 0.02);
            }

            for (auto &drive: getDriveComps())
            {
                drive.reset(spec.sampleRate, 0.02);
            }

            for (auto &mix: getMixes())
            {
                mix.reset(spec.sampleRate, 0.02);
            }

            m_tilt_eq.prepare(spec);
        }

        virtual void process(juce::dsp::AudioBlock<float>& block) = 0;

        void processBlock(juce::dsp::AudioBlock<float>& block)
        {
            m_tilt_eq.process(block);
        }

        virtual void setDrive(float newDrive) = 0;
        virtual void setMix(float newMix) = 0;
        void setTone(float newTone) { m_tilt_eq.set_tilt(newTone); };

        [[nodiscard]] float getInputComp() const { return m_input_comp; }
        [[nodiscard]] float getOutputComp() const { return m_output_comp; }

        std::array<juce::SmoothedValue<float>, num_channels>& getDrives() { return m_drive_smoothers; }
        std::array<juce::SmoothedValue<float>, num_channels>& getDriveComps() { return m_drive_comp_smoothers; }
        std::array<juce::SmoothedValue<float>, num_channels>& getMixes() { return m_mix_smoothers; }

    private:
        std::array<juce::SmoothedValue<float>, num_channels> m_drive_smoothers, m_mix_smoothers, m_drive_comp_smoothers;

        const float m_input_comp = juce::Decibels::decibelsToGain(18.0f);
        const float m_output_comp = juce::Decibels::decibelsToGain(-18.0f);
        viator::dsp::TiltEQ m_tilt_eq;
    };
}
