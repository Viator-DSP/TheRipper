//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>
#include "../Utils/Utils.h"

namespace viator::dsp
{
    class Distortion
    {
    public:
        Distortion();

        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::dsp::AudioBlock<float>& block);

        void setDrive(float newDrive);
    private:
        juce::dsp::ProcessSpec m_spec;
        std::array<juce::SmoothedValue<float>, 2> m_drive_smoothers;
    };
}
