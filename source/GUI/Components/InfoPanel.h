#pragma once
#include "BinaryData.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace viator::gui::components
{

class InfoPanel final : public juce::Component
{
 public:
    InfoPanel()
    {
        const auto shadow_color = juce::Colours::black;
        auto shadow = juce::DropShadow(shadow_color, 25, {});

        m_drop_shadow = std::make_unique<juce::DropShadower>(shadow);
        m_drop_shadow->setOwner(this);

        m_thanks_text_editor.setMultiLine(true);
        m_thanks_text_editor.setReadOnly(true);
        m_thanks_text_editor.setCaretVisible(false);
        m_thanks_text_editor.setText(
            "A special thanks to Michelangelo Nottoli and his great repository of SVG icons https://github.com/fefanto/fontaudio.");
        m_thanks_text_editor.setJustification(juce::Justification::centred);
        m_thanks_text_editor.setColour(juce::TextEditor::ColourIds::textColourId, viator::gui_utils::Colors::text());
        m_thanks_text_editor.setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
                                       juce::Colours::transparentBlack);
        m_thanks_text_editor.setColour(juce::TextEditor::ColourIds::highlightColourId, viator::gui_utils::Colors::text());
        m_thanks_text_editor.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        m_thanks_text_editor.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        m_thanks_text_editor.applyColourToAllText(viator::gui_utils::Colors::text());
        addAndMakeVisible(m_thanks_text_editor);

        setButtonProps(m_patreon_button,
                       BinaryData::patreonsvgrepocom_svg,
                       BinaryData::patreonsvgrepocom_svgSize,
                       "Patreon",
                       juce::Colour(255, 66, 77),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_patreon_button->onClick = [this]()
        {
            launch("https://www.patreon.com/ViatorDSP");
        };

        setButtonProps(m_gumroad_button,
                       BinaryData::gumroadsvgrepocom_svg,
                       BinaryData::gumroadsvgrepocom_svgSize,
                       "Gumroad",
                       juce::Colour(0, 0, 0),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_gumroad_button->onClick = [this]()
        {
            launch("https://viatordsp.gumroad.com/");
        };

        setButtonProps(m_discord_button,
                       BinaryData::discordaltsvgrepocom_svg,
                       BinaryData::discordaltsvgrepocom_svgSize,
                       "Discord",
                       juce::Colour(0, 0, 0),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_discord_button->onClick = [this]()
        {
            launch("https://discord.gg/Rv5MEWgwwQ");
        };

        setButtonProps(m_manual_button,
                       BinaryData::brochuresvgrepocom_svg,
                       BinaryData::brochuresvgrepocom_svgSize,
                       "Manual",
                       juce::Colour(0, 0, 0),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_manual_button->onClick = [this]()
        {
            launch("https://github.com/Viator-DSP/Haze/blob/main/Manual.md");
        };

        setButtonProps(m_support_button,
                       BinaryData::supportsvgrepocom_svg,
                       BinaryData::supportsvgrepocom_svgSize,
                       "Support",
                       juce::Colour(128, 129, 132),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_support_button->onClick = [this]()
        {
            launch("viatordsp@gmail.com");
        };

        setButtonProps(m_license_button,
                       BinaryData::licensesvgrepocom_svg,
                       BinaryData::licensesvgrepocom_svgSize,
                       "License",
                       juce::Colour(0, 0, 0),
                       viator::gui_utils::Colors::widget_off(),
                       viator::gui_utils::Colors::widget_on());

        m_license_button->onClick = [this]()
        {
            launch("https://github.com/Viator-DSP/Haze/blob/main/EULA.md");
        };
    }

    void paint(juce::Graphics &g) override
    {
        // VARS
        g.fillAll(viator::gui_utils::Colors::info_bg_color());
        g.setFont(viator::gui_utils::Fonts::bold(static_cast<float>(getHeight()) * m_hero_text_size));
        auto bounds = getLocalBounds();

        // TITLE TEXT
        const auto title_bounds = bounds.removeFromTop(getHeight() / 4);
        g.setColour(viator::gui_utils::Colors::text());
        g.drawFittedText(juce::String(JUCE_PROJECT_NAME) + " " + juce::String(JUCE_PROJECT_VERSION),
                         title_bounds,
                         juce::Justification::centred, 2);

        // THANKS TEXT
        g.setFont(viator::gui_utils::Fonts::regular(static_cast<float>(getHeight()) * m_paragraph_text_size));
        g.drawFittedText(
            "A sincere thanks to all who use my plugins and support my creative journey. \r\nYour enthusiasm "
            "and feedback inspires me to keep innovating and improving. \r\nTogether, we are making the world "
            "of audio more vibrant and accessible.",
            title_bounds.translated(0, juce::roundToInt(getHeight() * 0.06)),
            juce::Justification::centredBottom,
            3);

        // TEXT FOR EACH ICON
        auto icons_bounds = bounds.removeFromTop(getHeight() / 4);
        const auto button_width = static_cast<int>(static_cast<float>(bounds.getWidth()) / 15.0f);
        g.setFont(viator::gui_utils::Fonts::bold(static_cast<float>(getHeight()) * m_minor_text_size));
        const auto textY = juce::roundToInt(getHeight() * 0.5);
        g.drawFittedText("Donate",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);

        icons_bounds.removeFromLeft(button_width);
        g.drawFittedText("Plugins",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);

        icons_bounds.removeFromLeft(button_width);
        g.drawFittedText("Discord",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);

        icons_bounds.removeFromLeft(button_width);
        g.drawFittedText("Manual",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);

        icons_bounds.removeFromLeft(button_width);
        g.drawFittedText("Support",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);

        icons_bounds.removeFromLeft(button_width);
        g.drawFittedText("EULA",
                         icons_bounds.removeFromLeft(button_width)
                             .withSizeKeepingCentre(button_width, button_width)
                             .withX(icons_bounds.getX() + button_width)
                             .withY(textY),
                         juce::Justification::centredBottom, 1);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        const auto button_width = static_cast<int>(static_cast<float>(bounds.getWidth()) / 15.0f);
        const auto space_leftover = getWidth() - button_width;
        const auto padding = static_cast<int>(static_cast<float>(space_leftover) / 7.0f);

        if (m_patreon_button)
        {
            m_patreon_button->setBounds(bounds.removeFromLeft(button_width)
                                          .withSizeKeepingCentre(button_width, button_width)
                                          .translated(padding, 0.0f));
        }

        if (m_gumroad_button)
        {
            bounds.removeFromLeft(button_width);
            m_gumroad_button->setBounds(bounds.removeFromLeft(button_width)
                                          .withSizeKeepingCentre(button_width, button_width)
                                          .translated(padding, 0.0f));
        }

        if (m_discord_button)
        {
            bounds.removeFromLeft(button_width);
            m_discord_button->setBounds(bounds.removeFromLeft(button_width)
                                          .withSizeKeepingCentre(button_width,
                                                                 static_cast<int>(static_cast<float>(button_width)
                                                                     * 0.8f))
                                          .translated(padding, 0.0f));
        }

        if (m_manual_button)
        {
            bounds.removeFromLeft(button_width);
            m_manual_button->setBounds(bounds.removeFromLeft(button_width)
                                         .withSizeKeepingCentre(button_width, button_width)
                                         .translated(padding, 0.0f));
        }

        if (m_support_button)
        {
            bounds.removeFromLeft(button_width);
            m_support_button->setBounds(bounds.removeFromLeft(button_width)
                                          .withSizeKeepingCentre(button_width, button_width)
                                          .translated(padding, 0.0f));
        }

        if (m_license_button)
        {
            bounds.removeFromLeft(button_width);
            m_license_button->setBounds(bounds.removeFromLeft(button_width)
                                          .withSizeKeepingCentre(button_width, button_width)
                                          .translated(padding, 0.0f));
        }

        m_thanks_text_editor.setBounds(0, juce::roundToInt(getHeight() * 0.88), getWidth(), getHeight() / 10);
        m_thanks_text_editor.applyFontToAllText(viator::gui_utils::Fonts::regular(static_cast<float>(getHeight()) * m_paragraph_text_size));
    }

 private:

    std::unique_ptr<juce::DropShadower> m_drop_shadow;

    static constexpr float m_hero_text_size {0.05f };
    static constexpr float m_paragraph_text_size {0.04f };
    static constexpr float m_minor_text_size {0.04f };

    std::unique_ptr<juce::DrawableButton> m_patreon_button,
        m_gumroad_button, m_discord_button, m_manual_button, m_support_button, m_license_button;

    juce::TextEditor m_thanks_text_editor;

    void setButtonProps(std::unique_ptr<juce::DrawableButton> &button,
                        const void *imageData,
                        const int dataSize,
                        const juce::String &name,
                        const juce::Colour originalColor,
                        const juce::Colour newColorOff,
                        const juce::Colour newColorOn)
    {
        button = std::make_unique<juce::DrawableButton>(name, juce::DrawableButton::ButtonStyle::ImageStretched);
        const std::unique_ptr<juce::Drawable> imageOff{
            juce::Drawable::createFromImageData(imageData, dataSize)};
        const std::unique_ptr<juce::Drawable> imageOn{
            juce::Drawable::createFromImageData(imageData, dataSize)};
        imageOff->replaceColour(originalColor, newColorOff);
        imageOn->replaceColour(originalColor, newColorOn);
        button->setImages(imageOff.get(), imageOn.get());

        addAndMakeVisible(button.get());
    }

    static void launch(const juce::String& link)
    {
        const juce::URL url(link);
        if ( url.launchInDefaultBrowser()) {
            DBG("Launching " << link);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};

}
