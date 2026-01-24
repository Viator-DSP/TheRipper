//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include "../Utils/Utils.h"
#include "../Blocks/Distortion/Class_B_Amp.h"
#include "../Blocks/Distortion/Class_A_Valve.h"
#include "../Blocks/Distortion/Circle_Map.h"
#include "../Blocks/Distortion/Tape.h"
#include "../Blocks/Distortion/Overdrive.h"
#include "../Blocks/Distortion/BitReducer.h"

namespace viator::dsp
{
    class Distortion
    {
    public:
        Distortion();

        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::dsp::AudioBlock<float>& block) const;

        void setDrive(float newDrive);
        void setMix(float newMix);

        enum class DistortionType
        {
            kClassBAmp,
            kClassAValve,
            kCircleMap,
            kTape,
            kOverdrive,
            kBitReducer
        };

        void setDistortionType(DistortionType newType);
        void setTone(float newTone);

        enum Type
        {
            kClassBAmp = 0,
            kClass_A_Valve,
            kCircleMap,
            kTape,
            kOverdrive,
            kBitReducer,
            num_distortions
        };

        float getDistortionCompensation();

    private:
        juce::dsp::ProcessSpec m_spec;
        viator::dsp::ClassBAmp m_class_b_amp;
        viator::dsp::Class_A_Valve m_class_a_valve;
        viator::dsp::CircleMap m_circle_map;
        viator::dsp::Tape m_tape;
        viator::dsp::Overdrive m_overdrive;
        viator::dsp::BitReducer m_bit_reducer;

        std::vector<std::unique_ptr<viator::dsp::BaseDistortion>> m_distortions;

        DistortionType m_distortion_type = DistortionType::kClassBAmp;

        std::array<juce::SmoothedValue<float>, 2> m_compensations;
    };
}
