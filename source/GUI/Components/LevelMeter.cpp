#include "LevelMeter.h"

#include "Engine/Globals/Globals.h"
#include "GUI/Utils/Colors.h"

namespace viator::gui
{
    void LevelMeter::paint(juce::Graphics& g)
    {
        const float rawDB = juce::Decibels::gainToDecibels(smoothedGainReduction, -60.0f);
        const float dBValue = juce::jlimit(-60.0f, 0.0f, rawDB); // cap at 0 dB

        const float W = getWidth();
        const float H = getHeight();

        g.fillAll({42, 44, 60});
        const float meterW = W;

        // Thresholds
        constexpr float greenTopDB  = -18.0f;
        constexpr float yellowTopDB = -3.0f;

        // Segment heights
        const float greenH  = H * 0.6f;
        const float yellowH = H * 0.25f;
        const float redH    = H * 0.15f;

        // Convert dB to normalized meter fill
        float fillNorm = 0.0f;
        if (dBValue <= greenTopDB)
            fillNorm = juce::jmap(dBValue, -60.0f, greenTopDB, 0.0f, 0.6f);
        else if (dBValue <= yellowTopDB)
            fillNorm = juce::jmap(dBValue, greenTopDB, yellowTopDB, 0.6f, 0.85f);
        else
            fillNorm = juce::jmap(dBValue, yellowTopDB, 0.0f, 0.85f, 1.0f);

        const float fillH = H * fillNorm;
        float remainingH = fillH;
        float startY = H;

        // Green
        if (remainingH > 0.0f)
        {
            const float segH = juce::jmin(remainingH, greenH);
            startY -= segH;
            g.setColour(viator::gui_utils::Colors::meter_green());
            g.fillRect(0.f, startY, meterW, segH);
            remainingH -= segH;
        }

        // Yellow
        if (remainingH > 0.0f)
        {
            const float segH = juce::jmin(remainingH, yellowH);
            startY -= segH;
            g.setColour(viator::gui_utils::Colors::meter_yellow());
            g.fillRect(0.f, startY, meterW, segH);
            remainingH -= segH;
        }

        // Red
        if (remainingH > 0.0f)
        {
            const float segH = juce::jmin(remainingH, redH);
            startY -= segH;
            g.setColour(viator::gui_utils::Colors::meter_red());
            g.fillRect(0.f, startY, meterW, segH);
        }
    }

    void LevelMeter::setLevel(const float newLevel)
    {
        smoothedGainReduction = newLevel;
        repaint();
    }
}
