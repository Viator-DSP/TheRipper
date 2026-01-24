//
// Created by Landon Viator on 12/5/25.
//

#include "Distortion.h"

namespace viator::dsp {
    Distortion::Distortion() {
        m_distortions.clear();
        m_distortions.emplace_back(std::make_unique<viator::dsp::ClassBAmp>());
        m_distortions.emplace_back(std::make_unique<viator::dsp::Class_A_Valve>());
        m_distortions.emplace_back(std::make_unique<viator::dsp::CircleMap>());
        m_distortions.emplace_back(std::make_unique<viator::dsp::Tape>());
        m_distortions.emplace_back(std::make_unique<viator::dsp::Overdrive>());
        m_distortions.emplace_back(std::make_unique<viator::dsp::BitReducer>());
    }

    void Distortion::prepare(const juce::dsp::ProcessSpec &spec) {
        m_spec = spec;

        for (const auto &distortion: m_distortions) {
            distortion->prepare(m_spec);
        }

        for (auto &smoother: m_compensations) {
            smoother.reset(spec.sampleRate, 0.02);
        }
    }

    void Distortion::process(juce::dsp::AudioBlock<float> &block) const
    {
        switch (m_distortion_type) {
            case DistortionType::kClassBAmp: m_distortions[kClassBAmp]->process(block);
                break;
            case DistortionType::kClassAValve: m_distortions[kClass_A_Valve]->process(block);
                break;
            case DistortionType::kCircleMap: m_distortions[kCircleMap]->process(block);
                break;
            case DistortionType::kTape: m_distortions[kTape]->process(block);
                break;
            case DistortionType::kOverdrive: m_distortions[kOverdrive]->process(block);
                break;
            case DistortionType::kBitReducer: m_distortions[kBitReducer]->process(block);
                break;
        }
    }

    void Distortion::setDrive(const float newDrive) {
        for (const auto &distortion: m_distortions) {
            distortion->setDrive(newDrive);
        }

        m_distortions[kClass_A_Valve]->setDrive(newDrive * 0.2f);

        const auto bit_depth = juce::jmap(newDrive, 0.0f, 30.0f, 16.0f, 2.0f);
        if (auto *distortion = dynamic_cast<viator::dsp::BitReducer *>(m_distortions[kBitReducer].get())) {
            distortion->setBitDepth(bit_depth);
        }

        for (auto &smoother: m_compensations) {
            switch (m_distortion_type) {
                case DistortionType::kClassBAmp: smoother.setTargetValue(juce::Decibels::decibelsToGain(newDrive * -0.5f));
                    break;
                case DistortionType::kClassAValve: smoother.setTargetValue(juce::Decibels::decibelsToGain(0.0f));
                    break;
                case DistortionType::kCircleMap: smoother.setTargetValue(juce::Decibels::decibelsToGain(0.0f));
                    break;
                case DistortionType::kTape: smoother.setTargetValue(juce::Decibels::decibelsToGain(0.0f));
                    break;
                case DistortionType::kOverdrive: smoother.setTargetValue(juce::Decibels::decibelsToGain(0.0f));
                    break;
                case DistortionType::kBitReducer: smoother.setTargetValue(juce::Decibels::decibelsToGain(0.0f));
                    break;
            }
            smoother.skip(m_spec.maximumBlockSize);
        }
    }

    void Distortion::setMix(const float newMix) {
        for (const auto &distortion: m_distortions) {
            distortion->setMix(newMix);
        }
    }

    void Distortion::setTone(const float newTone) {
        for (const auto &distortion: m_distortions) {
            distortion->setTone(newTone);
        }

        if (auto *distortion = dynamic_cast<viator::dsp::Overdrive *>(m_distortions[kBitReducer].get())) {
            distortion->setPeakDb(newTone);
        }
    }

    void Distortion::setDistortionType(const DistortionType newType) {
        m_distortion_type = newType;
    }

    float Distortion::getDistortionCompensation() {
        const auto combined_value = (m_compensations[0].getCurrentValue() + m_compensations[1].getCurrentValue()) *
                                    0.5f;
        return combined_value;
    }
}
