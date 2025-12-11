#pragma once

#include <BinaryData.h>
#include "../Utils/FilmStripImage.h"

namespace viator::gui::widgets
{
    class NeptuneVuMeter final : public juce::Component
    {
    public:
        NeptuneVuMeter()
        {
            dbToFrameTable = {
                    {-20.0f, 6.0f},
                    {-10.0f, 23.0f},
                    {-7.0f, 33.0f},
                    {-5.0f, 43.0f},
                    {-3.0f, 59.0f},
                    {-1.0f, 75.0f},
                    {0.0f, 87.0f},
                    {1.0f, 98.0f},
                    {2.0f, 112.0f},
                    {3.0f, 128.0f}
            };
        }

        void paint(juce::Graphics &g) override
        {
            auto bounds = getLocalBounds().toFloat();
            image.drawWithinCentered(g, getWidth(), getHeight(), normalizedLevel, bounds, 0, 0);
        }

        void setLevel(float db)
        {
            db = juce::jlimit(-20.0f, 5.0f, db);
            normalizedLevel = getNormalizedPositionForDb(db);
            repaint();
        }

    private:
        float normalizedLevel = 0.0f;

        FilmStripImage image{viator::gui_utils::Images::meter(), 129};

        std::vector<std::pair<float, float> > dbToFrameTable;

        float getNormalizedPositionForDb(float db) const
        {
            const auto frameCount = static_cast<float>(129 - 1);

            if (db <= dbToFrameTable.front().first) return 0.0f;
            if (db >= dbToFrameTable.back().first) return 1.0f;

            for (size_t i = 0; i < dbToFrameTable.size() - 1; ++i) {
                const auto &[db1, frame1] = dbToFrameTable[i];
                const auto &[db2, frame2] = dbToFrameTable[i + 1];

                if (db >= db1 && db <= db2) {
                    const float t = (db - db1) / (db2 - db1);
                    const float interpolatedFrame = juce::jmap(t, frame1, frame2);
                    return interpolatedFrame / frameCount;
                }
            }

            return 0.0f;
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeptuneVuMeter)
    };
}
