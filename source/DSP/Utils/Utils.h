//
// Created by Landon Viator on 12/5/25.
//

#pragma once

namespace viator::dsp_utils
{
    inline float mixSamples(const float xn, const float yn, const float mix_zero_to_one)
    {
        return (1.0f - mix_zero_to_one) * xn + yn * mix_zero_to_one;
    }

    static constexpr float two_by_pi = 2.0f / juce::MathConstants<float>::pi;

    inline float softClip(const float xn, const float drive)
    {
        return two_by_pi * std::atan(xn * drive) * 2.0f;
    }

    inline float hardClip(const float xn, const float drive)
    {
        return std::clamp(xn * drive, -1.0f, 1.0f);
    }
}
