#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace viator::gui
{

class LevelMeter : public juce::Component
{
public:
    LevelMeter() = default;

    void paint (juce::Graphics&) override;

    void setLevel(float newLevel);

    float getReduction()
    {
        return smoothedGainReduction;
    }

    enum class MeterDirection
    {
        kVertical,
        kHorizontal
    };

    void setMeterDirection(const MeterDirection direction) {meter_direction = direction; repaint(); }
    void setMeterDrawDirection(const bool newDirection) {drawRightToLeft = newDirection; repaint(); }

    void setIsIrMeter(bool isIRMeter) { is_ir_meter = isIRMeter; repaint(); }

private:

    float smoothedGainReduction = 0.0f;

    MeterDirection meter_direction = MeterDirection::kVertical;

    bool drawRightToLeft { false };

    bool is_ir_meter { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
}