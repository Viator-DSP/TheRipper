//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../Utils/Utils.h"
#include "../Blocks/Distortion/Class_B_Amp.h"
#include "../Blocks/Distortion/ClassAValve.h"

namespace viator::dsp
{
    class Distortion
    {
    public:
        Distortion();

        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::dsp::AudioBlock<float>& block);

        void setDrive(float newDrive);
        void setMix(float newMix);

        enum Channel
        {
            kLeft = 0,
            kRight,
            num_channels
        };

        enum class DistortionType
        {
            kClassBAmp,
            kClassAValve
        };

        void setDistortionType(DistortionType newType);

    private:
        juce::dsp::ProcessSpec m_spec;
        viator::dsp::ClassBAmp m_class_b_amp;
        viator::dsp::ClassAValve m_class_a_valve;

        DistortionType m_distortion_type = DistortionType::kClassBAmp;
    };
}
