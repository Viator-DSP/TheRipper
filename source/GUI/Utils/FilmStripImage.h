#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <utility>

struct FilmStripImage
{
    FilmStripImage (juce::Image  image, int frameCount_)
        : m_image (std::move(image)), m_frame_count (frameCount_)
    {
    }

    void drawWithin (juce::Graphics& g, int width, int height, float position) const
    {
        auto subImage = m_image.getClippedImage({0, getFrameYPosition(position), m_image.getWidth(), m_frame_height });
        g.drawImageWithin(subImage, 0, 0, width, height, juce::RectanglePlacement::stretchToFit);
    }

    void drawWithinCentered (juce::Graphics& g, int width, int height, float position, juce::Rectangle<float>& bounds, int xOffset, int yOffset) const
    {
        auto subImage = m_image.getClippedImage({0, getFrameYPosition(position), m_image.getWidth(), m_frame_height });
        const auto x = bounds.getWidth() / 2.0f - static_cast<float>(width) / 2.0f;
        const auto y = bounds.getHeight() / 2.0f - static_cast<float>(height) / 2.0f;
        g.drawImage(subImage,
                    bounds.withSizeKeepingCentre(static_cast<float>(width),
                                                 static_cast<float>(height)).withY(y + static_cast<float>(yOffset)).withX(x + static_cast<float>(xOffset)), juce::RectanglePlacement::stretchToFit);
    }

    void drawScaledInRectangle (juce::Graphics& g, const juce::Rectangle<float>& rect, float position) const
    {
        g.drawImage (m_image,
                     static_cast<int> (rect.getX()),
                     static_cast<int> (rect.getY()),
                     static_cast<int> (rect.getWidth()),
                     static_cast<int> (rect.getHeight()),
                     0,
                     getFrameYPosition(position),
                     m_image.getWidth(),
                     static_cast<int> (rect.getHeight()));
    }

    const juce::Image m_image;
    const int m_frame_count;
    const int m_frame_height = m_image.getHeight() / m_frame_count;

private:
    int getFrameYPosition(const float position) const
    {
        return m_frame_height * juce::roundToInt (position * (m_frame_count - 1));
    }

    template<typename T>
    [[nodiscard]] static int toInt(T value)
    {
        return static_cast<int>(std::round(value));
    }
};
