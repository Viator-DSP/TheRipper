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

        for (auto& drive : m_drive_smoothers) {
            drive.reset(spec.sampleRate, 0.02);
        }
    }

    void Distortion::process(juce::dsp::AudioBlock<float> &block)
    {

    }

    void Distortion::setDrive(const float newDrive)
    {
        for (auto& drive : m_drive_smoothers) {
            drive.setTargetValue(juce::Decibels::decibelsToGain(newDrive));
        }
    }

}