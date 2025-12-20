#pragma once

#include <juce_dsp/juce_dsp.h>

namespace viator::dsp
{
    class TiltEQ
    {
    public:
        TiltEQ() = default;

        void prepare(const juce::dsp::ProcessSpec &spec)
        {
            m_spec = spec;

            m_hp_tilt.prepare(m_spec);
            m_lp_tilt.prepare(m_spec);

            m_phase = 0.0f;

            updateFilters();
        }

        void set_tilt(const float new_tilt)
        {
            m_tilt = new_tilt;
            updateFilters();
        }

        void process(juce::dsp::AudioBlock<float> &block)
        {
            m_phase += m_rate_hz / static_cast<float>(m_spec.sampleRate);
            if (m_phase >= 1.0f)
                m_phase -= 1.0f;

            const float mod = std::sin(juce::MathConstants<float>::twoPi * m_phase) + m_drift_depth;
            m_effective_tilt_db = juce::jmap(mod, 1.0f, 2.0f, 1.0f, 1.01f);
            m_effective_tilt_hz = juce::jmap(mod, 1.0f, 2.0f, 1.0f, 1.1f);
            updateFilters();

            const juce::dsp::ProcessContextReplacing<float> context(block);
            m_hp_tilt.process(context);
            m_lp_tilt.process(context);
        }

        void setTiltPivot(const float new_tilt_pivot)
        {
            m_pivot_hz = new_tilt_pivot;
        }

    private:
        void updateFilters() const
        {
            const auto gain_db = m_tilt * m_effective_tilt_db;
            const auto high_gain = juce::Decibels::decibelsToGain(gain_db);
            const auto low_gain = juce::Decibels::decibelsToGain(-gain_db);

            *m_hp_tilt.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                    m_spec.sampleRate,
                    m_pivot_hz * m_effective_tilt_hz,
                    m_q,
                    high_gain
            );

            *m_lp_tilt.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                    m_spec.sampleRate,
                    m_pivot_hz * m_effective_tilt_hz,
                    m_q,
                    low_gain
            );
        }

        juce::dsp::ProcessSpec m_spec{};

        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> m_hp_tilt, m_lp_tilt;

        float m_tilt = 0.0f;
        float m_effective_tilt_db = 1.0f;
        float m_effective_tilt_hz = 1.0f;

        float m_phase = 0.0f;
        const float m_rate_hz = 1.0f;
        const float m_drift_depth = 1.0f;

        float m_pivot_hz = 1000.0f;
        const float m_q = 0.707f;
    };
}