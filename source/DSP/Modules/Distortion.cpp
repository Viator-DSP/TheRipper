//
// Created by Landon Viator on 12/5/25.
//

#include "Distortion.h"

namespace viator::dsp
{
    Distortion::Distortion()
    {

    }

    void Distortion::prepare(const juce::dsp::ProcessSpec& spec)
    {
        m_spec = spec;

        for (auto& filter : m_pink_noise_filter)
        {
            filter.prepare(m_spec);
            filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
            filter.setCutoffFrequency(1000.0f);
        }

        m_class_b_amp.prepare(m_spec);
        m_class_a_valve.prepare(m_spec);
        m_circle_map.prepare(m_spec);
        m_tape.prepare(m_spec);
        m_overdrive.prepare(m_spec);
    }

    void Distortion::process(juce::dsp::AudioBlock<float> &block)
    {
        for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
            auto *data = block.getChannelPointer(channel);
            for (size_t sample = 0; sample < block.getNumSamples(); ++sample) {
                const float xn = data[sample];
                const float noise = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
                const float yn = xn + m_pink_noise_filter[channel].processSample(static_cast<int>(channel), noise * 0.0003f);
                data[sample] = yn;
            }
        }

        switch(m_distortion_type)
        {
            case DistortionType::kClassBAmp: m_class_b_amp.process(block); break;
            case DistortionType::kClassAValve: m_class_a_valve.process(block); break;
            case DistortionType::kCircleMap: m_circle_map.process(block); break;
            case DistortionType::kTape: m_tape.process(block); break;
            case DistortionType::kOverdrive: m_overdrive.process(block); break;
        }
    }

    void Distortion::setDrive(const float newDrive)
    {
        m_class_b_amp.setDrive(newDrive);
        m_class_a_valve.setDrive(newDrive * 0.2f);
        m_circle_map.setDrive(newDrive);
        m_tape.setDrive(newDrive);
        m_overdrive.setDrive(newDrive);
    }

    void Distortion::setMix(const float newMix)
    {
        m_class_b_amp.setMix(newMix);
        m_class_a_valve.setMix(newMix);
        m_circle_map.setMix(newMix);
        m_tape.setMix(newMix);
        m_overdrive.setMix(newMix);
    }

    void Distortion::setTone(const float newTone)
    {
        m_class_b_amp.setTone(newTone);
        m_class_a_valve.setTone(newTone);
        m_circle_map.setTone(newTone);
        m_tape.setTone(newTone);
        m_overdrive.setTone(newTone);
        m_overdrive.setPeakDb(newTone);
    }

    void Distortion::setDistortionType(const DistortionType newType)
    {
        m_distortion_type = newType;
    }
}