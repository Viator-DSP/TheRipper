//
// Created by Landon Viator on 12/1/25.
//

#pragma once
#include "../Utils/PresetManager.h"
#include "../Utils/Colors.h"

namespace viator::gui::components
{
    class PresetBrowser final : public juce::Component {

    public:
        PresetBrowser(juce::AudioProcessorValueTreeState&, juce::ValueTree&);
        ~PresetBrowser();

        void paint(juce::Graphics &g) override;
        void resized() override;

        enum Button
        {
            kLeft = 0,
            kRight,
            numButtons
        };

    private:
        viator::gui_utils::PresetManager m_preset_manager;

        std::array<juce::TextButton, numButtons> m_buttons;
        void setButtonProps();

        juce::ComboBox m_menu;
        void setMenuProps();

        void refreshMenu();
        void save(const juce::String& folder_path);
        void load(const juce::String& folder_path);

        juce::Array<juce::File> m_xml_files;
        juce::StringArray m_xml_file_paths;

        std::unique_ptr<juce::FileChooser> m_chooser;

        int getXMLByName(const juce::String& name) const;

        int m_previous_menu_id {-1000};

        void refreshAll(const juce::String& name);

        void navigate_left();
        void navigate_right();
    };
}