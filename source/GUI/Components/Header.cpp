//
// Created by Landon Viator on 11/24/25.
//

#include "Header.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace viator::gui::components
{
    Header::Header(juce::AudioProcessorValueTreeState &t, juce::ValueTree& v) : m_tree_state(t), m_preset_browser_component(t, v)
    {
        // MENUS
        const auto oversampling_items = viator::globals::Oversampling::items;
        const auto stereo_items = viator::globals::MidSide::items;
        setComboBoxProps(m_oversampling_menu, oversampling_items);
        setComboBoxProps(m_preset_browser, juce::StringArray{});
        setComboBoxProps(m_midside_menu, stereo_items);

        m_oversampling_attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
                (m_tree_state, viator::globals::PluginParameters::oversamplingID, m_oversampling_menu);

        m_mid_side_attach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
                (m_tree_state, viator::globals::PluginParameters::midSideID, m_midside_menu);

        // BUTTONS
        setTextButtonProps(m_preset_a_button, "A");
        setTextButtonProps(m_preset_b_button, "B");

        m_preset_a_button.onClick = [this]()
        {
            sendActionMessage(viator::globals::ActionMessages::presetAToggled);
        };

        m_preset_b_button.onClick = [this]()
        {
            sendActionMessage(viator::globals::ActionMessages::presetBToggled);
        };

        m_preset_a_button.setToggleState(true, juce::dontSendNotification);

        // SETTINGS
        const std::unique_ptr<juce::Drawable> settingsOff
        {
            juce::Drawable::createFromImageData(BinaryData::fadhexpand_svg,
                                                BinaryData::fadhexpand_svgSize)
        };
        const std::unique_ptr<juce::Drawable> settingsOn
        {
            juce::Drawable::createFromImageData(BinaryData::fadhexpand_svg,
                                                BinaryData::fadhexpand_svgSize)
        };
        settingsOff->replaceColour(juce::Colour(0, 0, 0), viator::gui_utils::Colors::widget_off());
        settingsOn->replaceColour(juce::Colour(0, 0, 0), viator::gui_utils::Colors::power_on());
        setSVGButtonProps(m_settings_button, settingsOff.get(), settingsOn.get(), "Settings");

        m_settings_button->onClick = [this]
        {
            sendActionMessage(viator::globals::ActionMessages::infoToggled);
        };

        // POWER
        const std::unique_ptr<juce::Drawable> powerOff
        {
            juce::Drawable::createFromImageData(BinaryData::fadpowerswitch_svg,
                                                BinaryData::fadpowerswitch_svgSize)
        };
        const std::unique_ptr<juce::Drawable> powerOn
        {
            juce::Drawable::createFromImageData(BinaryData::fadpowerswitch_svg,
                                                BinaryData::fadpowerswitch_svgSize)
        };
        powerOff->replaceColour(juce::Colours::black, viator::gui_utils::Colors::widget_off());
        powerOn->replaceColour(juce::Colours::black, viator::gui_utils::Colors::power_on());
        setSVGButtonProps(m_power_button, powerOff.get(), powerOn.get(), "Power");

        m_power_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
                (m_tree_state, viator::globals::PluginParameters::globalPowerID, *m_power_button);

        addAndMakeVisible(m_preset_browser_component);
    }

    Header::~Header()
    {
        m_preset_browser.setLookAndFeel(nullptr);
        m_midside_menu.setLookAndFeel(nullptr);
        m_oversampling_menu.setLookAndFeel(nullptr);

        m_preset_a_button.setLookAndFeel(nullptr);
        m_preset_b_button.setLookAndFeel(nullptr);
    }

    void Header::paint(juce::Graphics &g)
    {
        g.fillAll(viator::gui_utils::Colors::editor_bg_color());

        constexpr auto x = 0;
        constexpr auto y = 0;
        const auto height = juce::roundToInt(getHeight() * 0.9);
        const auto width = juce::roundToInt(getWidth() * 0.246);
        g.drawImageWithin(viator::gui_utils::Images::logo(), x, getHeight() / 2 - height / 2, width, height,
                          juce::RectanglePlacement::stretchToFit);
    }

    void Header::resized()
    {
        // MENUS
        const auto height = juce::roundToInt(getHeight() * 0.5);
        const auto y = height / 2;
        auto width = juce::roundToInt(getWidth() * 0.175);
        const auto x = getWidth() / 2 - width / 2;
        m_preset_browser_component.setBounds(x, y, width, height);
        width = juce::roundToInt(getWidth() * 0.0875);
        m_oversampling_menu.setBounds(m_preset_browser_component.getRight() + y, y, width, height);
        m_midside_menu.setBounds(m_oversampling_menu.getRight() + y, y, width, height);

        // BUTTONS
        width = juce::roundToInt(getWidth() * 0.029);
        const auto y_padding = juce::roundToInt(getWidth() * 0.014);
        m_preset_a_button.setBounds(m_midside_menu.getRight() + y_padding, y, width, height);
        m_preset_b_button.setBounds(m_preset_a_button.getRight(), y, width, height);
        m_settings_button->setBounds(getWidth() - width - y_padding, y, width, height);
        m_power_button->setBounds(m_settings_button->getX() - width * 2 - y_padding, y, width, height);
    }

    void Header::setComboBoxProps(juce::ComboBox &box, const juce::StringArray &items)
    {
        if (!items.isEmpty()) {
            box.addItemList(items, 1);
            box.setSelectedId(1, juce::dontSendNotification);
        }

        box.setLookAndFeel(&m_menu_laf);
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, viator::gui_utils::Colors::widget_center());
        box.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
        viator::gui_utils::Colors::widget_dark());
        box.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
        viator::gui_utils::Colors::widget_on());
        box.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(box);
    }

    void Header::setTextButtonProps(juce::TextButton &button, const juce::String &text)
    {
        button.setClickingTogglesState(true);
        button.setButtonText(text);
        button.setRadioGroupId(1000);
        button.setColour(juce::TextButton::ColourIds::buttonColourId,
                         viator::gui_utils::Colors::widget_center());
        button.setColour(juce::TextButton::ColourIds::buttonOnColourId,
                         viator::gui_utils::Colors::power_on());
        button.setColour(juce::TextButton::ColourIds::textColourOnId, viator::gui_utils::Colors::text());
        button.setColour(juce::TextButton::ColourIds::textColourOffId, viator::gui_utils::Colors::text());
        button.setColour(juce::ComboBox::ColourIds::outlineColourId,
                         viator::gui_utils::Colors::widget_center());
        button.setLookAndFeel(&m_text_button_laf);
        addAndMakeVisible(button);
    }

    void Header::setSVGButtonProps(std::unique_ptr<juce::DrawableButton> &button,
                                   const juce::Drawable *offImage,
                                   const juce::Drawable *onImage,
                                   const juce::String &name)
    {
        button = std::make_unique<juce::DrawableButton>(name,
                                                        juce::DrawableButton::ButtonStyle::ImageStretched);
        button->setClickingTogglesState(true);
        button->setImages(offImage, nullptr, nullptr, nullptr, onImage);
        button->setColour(juce::DrawableButton::ColourIds::backgroundColourId, juce::Colour());
        button->setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, juce::Colour());
        addAndMakeVisible(button.get());
    }
}
