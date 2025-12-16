//
// Created by Landon Viator on 11/24/25.
//

#pragma once

#include "Modules/Distortion.h"

namespace viator::dsp
{
    class ProcessBlock
    {
    public:
        ProcessBlock() = default;

        ~ProcessBlock() = default;

        enum Filters
        {
            kHP = 0,
            kLP,
            num_filters
        };

        enum class FilterMode
        {
            kDigital,
            kAnalog
        };

        enum class RipMode
        {
            kNormal,
            kRip
        };

        void prepare(const double sample_rate, const int samples_per_block, const int num_channels, int factor)
        {
            juce::dsp::ProcessSpec spec{};
            spec.sampleRate = sample_rate;
            spec.maximumBlockSize = samples_per_block;
            spec.numChannels = num_channels;

            m_oversampler = std::make_unique<juce::dsp::Oversampling<float>>(spec.numChannels,
                                                                             factor,
                                                                             juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
                                                                             true);
            m_oversampler->initProcessing(spec.maximumBlockSize);

            for (auto &filter: m_filters)
            {
                filter.prepare(spec);
            }

            m_filters[kHP].setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            m_filters[kLP].setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

            for (auto &filter: m_analog_filters)
            {
                filter.prepare(spec);
            }

            m_analog_filters[kHP].setMode(juce::dsp::LadderFilterMode::HPF24);
            m_analog_filters[kLP].setMode(juce::dsp::LadderFilterMode::LPF24);

            const int realFactor = 1 << factor;
            juce::dsp::ProcessSpec osSpec = spec;
            osSpec.sampleRate      *= realFactor;
            osSpec.maximumBlockSize *= realFactor;

            m_distortion.prepare(osSpec);
        }

        void process(juce::AudioBuffer<float> &buffer, const int num_samples)
        {
            const int num_oversampled_samples = num_samples * static_cast<int>(m_oversampler->getOversamplingFactor());
            juce::dsp::AudioBlock<float> block(buffer);

            // FILTERS
            switch (m_filter_mode)
            {
                case FilterMode::kDigital:
                {
                    for (auto &filter: m_filters)
                    {
                        filter.process(juce::dsp::ProcessContextReplacing<float>(block));
                    } break;
                }

                case FilterMode::kAnalog:
                {
                    for (auto &filter : m_analog_filters)
                    {
                        filter.process(juce::dsp::ProcessContextReplacing<float>(block));
                    }

                    block.multiplyBy(1.758f);

                    break;
                }
            }


            auto up_sampled_block = m_oversampler->processSamplesUp(block);

            // RIP
            if (m_rip_mode == RipMode::kRip)
            {
                up_sampled_block.multiplyBy(10.0f);
            }

            m_distortion.process(up_sampled_block);

            // RIP BACK DOWN
            if (m_rip_mode == RipMode::kRip)
            {
                up_sampled_block.multiplyBy(0.1f);
            }

            m_oversampler->processSamplesDown(block);
        }

        void updateParameters(const viator::globals::PluginParameters::parameters &parameters)
        {
            const auto hp_cutoff = parameters.hpParam->get();
            m_filters[kHP].setCutoffFrequency(hp_cutoff);
            m_analog_filters[kHP].setCutoffFrequencyHz(hp_cutoff);

            const auto lp_cutoff = parameters.lpParam->get();
            m_filters[kLP].setCutoffFrequency(lp_cutoff);
            m_analog_filters[kLP].setCutoffFrequencyHz(lp_cutoff);

            const auto is_steep = parameters.analogParam->get();
            m_filter_mode = static_cast<FilterMode>(static_cast<int>(is_steep));

            const auto is_rip = parameters.ripParam->get();
            m_rip_mode = static_cast<RipMode>(static_cast<int>(is_rip));

            const auto drive = parameters.driveParam->get();
            const auto mix = parameters.mixParam->get();
            const auto type = parameters.typeParam->get();
            const auto tone = parameters.toneParam->get();
            m_distortion.setDrive(drive);
            m_distortion.setMix(mix);
            m_distortion.setDistortionType(static_cast<viator::dsp::Distortion::DistortionType>(type));
            m_distortion.setTone(tone);
        }

    private:
        std::unique_ptr<juce::dsp::Oversampling<float>> m_oversampler;

        std::array<juce::dsp::LinkwitzRileyFilter<float>, num_filters> m_filters;
        std::array<juce::dsp::LadderFilter<float>, num_filters> m_analog_filters;

        FilterMode m_filter_mode = FilterMode::kDigital;
        RipMode m_rip_mode = RipMode::kNormal;

        viator::dsp::Distortion m_distortion;
    };
}

