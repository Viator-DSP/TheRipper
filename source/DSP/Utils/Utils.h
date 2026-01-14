//
// Created by Landon Viator on 12/5/25.
//

#pragma once
#include <juce_dsp/juce_dsp.h>

namespace viator::dsp_utils
{
    inline float mixSamples(const float xn, const float yn, const float mix_zero_to_one)
    {
        return (1.0f - mix_zero_to_one) * xn + yn * mix_zero_to_one;
    }

    static constexpr float two_by_pi = 2.0f / juce::MathConstants<float>::pi;
    static constexpr float two_pi = 2.0f * juce::MathConstants<float>::pi;
    static const float input_comp = juce::Decibels::decibelsToGain(18.0f);
    static const float output_comp = juce::Decibels::decibelsToGain(-18.0f);

    inline float softClip(const float xn, const float drive)
    {
        return two_by_pi * std::atan(xn * 0.1f * drive) * 15.5f;
    }

    inline float hardClip(const float xn, const float drive, const float ceiling = 1.0f)
    {
        return std::clamp(xn * drive, ceiling * -1.0f, ceiling);
    }

    [[maybe_unused]] inline float arraya(const float xn)
    {
        if (!std::isfinite(xn))
            return 0.0f;

        const auto yn = std::clamp(xn, -4.0f, 4.0f);

        return yn * 1.5f * (1.0f - (yn * yn) / 3.0f);
    }

    inline float circleMapWaveshaper(const float xn, const float drive)
    {
        return xn + (drive / two_pi) * std::sin(xn * two_pi);
    }

    inline float polettiWaveshaper(const float xn, const float drive, const float ln, const float lp)
    {
        constexpr float eps = 1.0e-12f;
        const float numerator = xn * drive;
        float denomNeg = 1.0f - numerator / ln;
        float denomPos = 1.0f + numerator / lp;

        // “soft sign epsilon injection” for stability
        denomNeg += copysignf(eps, denomNeg);
        denomPos += copysignf(eps, denomPos);

        const float negative = numerator / denomNeg;
        const float positive = numerator / denomPos;

        const auto mask = static_cast<float>(xn >= 0.0f);

        return negative + (positive - negative) * mask;
    }

    inline float valveGridConduction(const float xn, const float thresh)
    {
        const auto mask = static_cast<float>(xn >= 0.0f);
        const float xn_n = xn;

        float clip_delta = xn - thresh;
        clip_delta = std::fmax(clip_delta, 0.0f);

        const float compression_factor = 0.4473253f + 0.5451584f *
                juce::dsp::FastMathApproximations::exp(-0.3241584f * clip_delta);

        const float xn_p = xn * compression_factor;

        return xn_n + (xn_p - xn_n) * mask;
    }

    inline float classAValve(float xn,
                             const float k,
                             const float thresh,
                             const float clipPos,
                             const float clipNeg)
    {
        float yn = 0.0f;

        if (xn > thresh)
        {
            if (xn > clipPos)
            {
                yn = clipPos;
            } else
            {
                xn -= thresh;

                constexpr float eps = 1.0e-12f;
                const auto is_zero = clipPos - thresh == 0.0f;
                xn /= is_zero ? (clipPos - thresh) : (clipPos - thresh + eps);

                yn = arraya(xn);
                yn *= (clipPos - thresh);
                yn += thresh;
            }
        } else if (xn > 0.0f)
        {
            yn = xn;
        } else
        {
            if (xn < clipNeg)
            {
                yn = clipNeg;
            } else
            {
                xn /= std::fabs(clipNeg);
                yn = std::tanh(xn * k) / tanh(k);
                yn *= std::fabs(clipNeg);
            }
        }

        return yn;
    }
}
