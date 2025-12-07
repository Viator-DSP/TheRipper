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

        m_class_b_amp.prepare(m_spec);
        m_class_a_valve.prepare(m_spec);
        m_circle_map.prepare(m_spec);
        m_tape.prepare(m_spec);
    }

    void Distortion::process(juce::dsp::AudioBlock<float> &block)
    {
        switch(m_distortion_type)
        {
            case DistortionType::kClassBAmp: m_class_b_amp.process(block); break;
            case DistortionType::kClassAValve: m_class_a_valve.process(block); break;
            case DistortionType::kCircleMap: m_circle_map.process(block); break;
            case DistortionType::kTape: m_tape.process(block); break;
        }
    }

    void Distortion::setDrive(const float newDrive)
    {
        m_class_b_amp.setDrive(newDrive);
        m_class_a_valve.setDrive(newDrive * 0.2f);
        m_circle_map.setDrive(newDrive);
        m_tape.setDrive(newDrive);
    }

    void Distortion::setMix(const float newMix)
    {
        m_class_b_amp.setMix(newMix);
        m_class_a_valve.setMix(newMix);
        m_circle_map.setMix(newMix);
        m_tape.setMix(newMix);
    }

    void Distortion::setDistortionType(const DistortionType newType)
    {
        m_distortion_type = newType;
    }
}