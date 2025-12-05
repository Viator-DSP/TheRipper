//
// Created by Landon Viator on 12/1/25.
//

#include "PresetBrowser.h"

#include "Engine/Globals/Globals.h"

namespace viator::gui::components
{
    PresetBrowser::PresetBrowser(juce::AudioProcessorValueTreeState &t,
                                 juce::ValueTree &v) : m_preset_manager(t, v)
    {
        setButtonProps();
        setMenuProps();

        m_preset_manager.createPresetFolder();
        refreshMenu();
    }

    PresetBrowser::~PresetBrowser()
    {
    }

    void PresetBrowser::paint(juce::Graphics &g)
    {
        g.setColour(viator::gui_utils::Colors::widget_center());

        const auto reduction = static_cast<float>(getHeight()) / 18.0f;
        g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(reduction), 6.0f);
    }

    void PresetBrowser::resized()
    {
        const auto button_width = getWidth() / 6;
        const auto button_height = getHeight() - 4;
        m_buttons[kLeft].setBounds(2, 2, button_width, button_height);
        m_buttons[kRight].setBounds(getWidth() - button_width - 2, 2, button_width, button_height);

        const auto box_width = m_buttons[kRight].getX() - m_buttons[kLeft].getRight() - 4;
        m_menu.setBounds(m_buttons[kLeft].getRight() + 2, 2, box_width, button_height);
    }

    void PresetBrowser::setButtonProps()
    {
        for (auto &button: m_buttons) {
            button.setClickingTogglesState(false);
            button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
            button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::transparentBlack);
            button.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
            addAndMakeVisible(button);
        }

        m_buttons[kLeft].setButtonText("<");
        m_buttons[kRight].setButtonText(">");

        m_buttons[kLeft].onClick = [this]()
        {
            navigate_left();
        };
        m_buttons[kRight].onClick = [this]()
        {
            navigate_right();
        };
    }

    void PresetBrowser::setMenuProps()
    {
        m_menu.addItem("Preset", 1);
        m_menu.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        m_menu.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        m_menu.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::backgroundColourId,
        viator::gui_utils::Colors::widget_dark());
        m_menu.getLookAndFeel().setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId,
        viator::gui_utils::Colors::widget_on());
        m_menu.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(m_menu);
        m_menu.setTextWhenNothingSelected("Presets");

        m_menu.onChange = [this]()
        {
            const auto not_negative = m_menu.getSelectedId() >= 1;
            const auto preset_changed = m_menu.getSelectedId() != m_previous_menu_id;
            if (not_negative && preset_changed) {
                m_previous_menu_id = m_menu.getSelectedId();
                m_preset_manager.loadPreset(m_xml_file_paths[m_menu.getSelectedItemIndex()]);
            }
        };
    }

    void PresetBrowser::refreshMenu()
    {
        m_xml_files.clear();
        m_menu.clear();

        juce::PopupMenu user_menu;

        auto folder_path = m_preset_manager.getPresetFolderLocation();
        const auto preset_folder = juce::File(folder_path);
        preset_folder.findChildFiles(m_xml_files, juce::File::TypesOfFileToFind::findFiles, false, "*.xml");
        m_xml_files.sort();

        for (int i = 0; i < m_xml_files.size(); ++i) {
            std::unique_ptr<juce::XmlElement> xml_document = juce::XmlDocument::parse(m_xml_files[i]);

            if (xml_document != nullptr) {
                juce::String root_element_name = xml_document->getTagName();

                if (root_element_name == m_preset_manager.getProjectName()) {
                    m_xml_file_paths.insert(i, m_xml_files[i].getFullPathName());
                    const juce::String fileName = m_xml_files[i].getFileNameWithoutExtension();
                    user_menu.addItem(juce::PopupMenu::Item(fileName).setID(i + 1));
                }
            }
        }

        m_menu.getRootMenu()->addSubMenu("User", user_menu);
        //populateFactoryMenu();

        m_menu.getRootMenu()->addSeparator();
        m_menu.getRootMenu()->addItem(juce::PopupMenu::Item("Save").setAction(
            [this, folder_path]()
            {
                save(folder_path);
            }
        ));
        m_menu.getRootMenu()->addItem(juce::PopupMenu::Item("Load").setAction(
            [this, folder_path]()
            {
                load(folder_path);
            }
        ));
        m_menu.getRootMenu()->addItem(juce::PopupMenu::Item("Set Location").setAction(
            [this]()
            {
                m_chooser = std::make_unique<juce::FileChooser>(
                    "Please select the location to save your preset...");

                const auto folder_chooser_flags = juce::FileBrowserComponent::canSelectDirectories;

                m_chooser->launchAsync(folder_chooser_flags, [this](const juce::FileChooser &chooser)
                {
                    juce::File file(chooser.getResult());

                    if (file.isDirectory()) {
                        m_preset_manager.setPresetFolderLocation(file.getFullPathName());
                        m_preset_manager.getVariableTree().setProperty(
                            viator::globals::TreeItems::presetFolder, file.getFullPathName(), nullptr);
                        refreshMenu();
                    }
                });
            }
        ));
    }

    void PresetBrowser::save(const juce::String &folder_path)
    {
        m_chooser =
                std::make_unique<juce::FileChooser>(
                    "Please select the location to save your preset...", folder_path, "*.xml");

        constexpr auto folder_chooser_flags = juce::FileBrowserComponent::saveMode;

        juce::String choice_name = "";

        m_chooser->launchAsync(folder_chooser_flags,
                               [this, choice_name](const juce::FileChooser &chooser)
                               {
                                   juce::File file(chooser.getResult());

                                   const juce::String file_name = file.getFileName();
                                   if (!file_name.endsWith(".xml")) {
                                       file = file.withFileExtension(".xml");
                                   }

                                   const auto &name = file.getFullPathName();

                                   if (name != "") {
                                       m_preset_manager.savePreset(file);
                                       refreshAll(file.getFileName());
                                   }
                               });
    }

    void PresetBrowser::load(const juce::String &folder_path)
    {
        m_chooser =
                std::make_unique<juce::FileChooser>("Please select the location to save your preset...",
                                                    folder_path, "*.xml");

        auto folder_chooser_flags = juce::FileBrowserComponent::openMode |
                                  juce::FileBrowserComponent::canSelectFiles;

        m_chooser->launchAsync(folder_chooser_flags, [this](const juce::FileChooser &chooser)
        {
            juce::File file(chooser.getResult());
            m_preset_manager.loadPreset(file.getFullPathName());
            refreshAll(file.getFileName());
        });
    }

    int PresetBrowser::getXMLByName(const juce::String &name) const
    {
        int id;

        for (int i = 0; i < m_xml_files.size(); ++i) {
            if (m_xml_files[i].getFileName() == name) {
                id = i + 1;
            }
        }

        jassert(id > 0);
        return id;
    }

    void PresetBrowser::refreshAll(const juce::String& name)
    {
        refreshMenu();
        const auto choice = getXMLByName(name);
        m_menu.setSelectedId(choice);
    }

    void PresetBrowser::navigate_left()
    {
        const auto currentId = m_menu.getSelectedId();
        const auto size = m_menu.getNumItems();
        constexpr auto skipChoices = 2;

        if (size <= skipChoices)
            return;

        constexpr auto decrementFactor = 1;

        const int new_choice = (currentId <= decrementFactor)
            ? size - skipChoices - decrementFactor
            : currentId - decrementFactor;

        m_menu.setSelectedId(new_choice);
    }

    void PresetBrowser::navigate_right()
    {
        const auto currentId = m_menu.getSelectedId();
        const auto size = m_menu.getNumItems();
        constexpr auto skipChoices = 2;

        if (size <= skipChoices)
            return;

        constexpr auto decrementFactor = 1;
        constexpr auto incrementFactor = 1;

        const int new_choice = (currentId >= size - skipChoices - decrementFactor)
                    ? incrementFactor
                    : currentId + incrementFactor;

        m_menu.setSelectedId(new_choice);
    }
}
