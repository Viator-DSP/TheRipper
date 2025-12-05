#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <functional>

namespace viator::dsp
{
    class MidSideProcessor
    {
    public:
        enum class StereoMode
        {
            kStereo,
            kMid,
            kSides
        };

        MidSideProcessor() = default;

        void prepare(const juce::dsp::ProcessSpec &spec)
        {
            constexpr auto num_channels = 1;
            const auto num_samples = static_cast<int>(spec.maximumBlockSize);
            midBuffer.setSize(num_channels, num_samples);
            midBuffer.clear();

            sideBuffer.setSize(num_channels, num_samples);
            sideBuffer.clear();
        }

        void setMode(const StereoMode newMode)
        {
            stereoMode = newMode;
        }

        void setBypass(const bool shouldBypass)
        {
            bypass = shouldBypass;
        }

        void process(juce::AudioBuffer<float> &ioBuffer,
                     const std::function<void(juce::AudioBuffer<float> &, int)> &processor)
        {
            if (ioBuffer.getNumChannels() != 2 || bypass)
                return;

            decodeBuffer(ioBuffer);

            switch (stereoMode) {
                case StereoMode::kStereo:
                    processor(ioBuffer, ioBuffer.getNumSamples());
                    break;
                case StereoMode::kMid:
                    processor(midBuffer, ioBuffer.getNumSamples());
                    break;
                case StereoMode::kSides:
                    processor(sideBuffer, ioBuffer.getNumSamples());
                    break;
            }

            encodeBuffer(ioBuffer);
        }

    private:
        void decodeBuffer(const juce::AudioBuffer<float> &buffer)
        {
            const int numSamples = buffer.getNumSamples();
            const float *left = buffer.getReadPointer(0);
            const float *right = buffer.getReadPointer(1);

            float *mid = midBuffer.getWritePointer(0);
            float *side = sideBuffer.getWritePointer(0);

            for (int i = 0; i < numSamples; ++i) {
                const float m = 0.5f * (left[i] + right[i]);
                const float s = 0.5f * (left[i] - right[i]);

                mid[i] = m;
                side[i] = s;
            }
        }

        void encodeBuffer(juce::AudioBuffer<float> &buffer) const
        {
            const int numSamples = buffer.getNumSamples();
            float *left = buffer.getWritePointer(0);
            float *right = buffer.getWritePointer(1);

            const float *mid = midBuffer.getReadPointer(0);
            const float *side = sideBuffer.getReadPointer(0);

            switch (stereoMode) {
                case StereoMode::kMid:
                case StereoMode::kSides:
                    for (int i = 0; i < numSamples; ++i) {
                        left[i] = mid[i] + side[i];
                        right[i] = mid[i] - side[i];
                    }
                    break;

                case StereoMode::kStereo:
                default:
                    break;
            }
        }

        StereoMode stereoMode = StereoMode::kStereo;
        juce::AudioBuffer<float> midBuffer;
        juce::AudioBuffer<float> sideBuffer;

        bool bypass{false};
    };
}
