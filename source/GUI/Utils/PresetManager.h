//
// Created by Landon Viator on 12/1/25.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace viator::gui_utils
{
    class PresetManager {
    public:
        explicit PresetManager(juce::AudioProcessorValueTreeState&, juce::ValueTree&);
        ~PresetManager();

        void savePreset(const juce::File& fileToSave) const;
        void loadPreset(const juce::String &filePath) const;

        void setProjectName(const juce::String& project_name) { m_project_name = project_name; };
        void setPresetFolderLocation(const juce::String& folder_name) { m_folder_location = folder_name; };
        void createPresetFolder() const;
        const juce::String& getPresetFolderLocation() { return m_folder_location; }
        [[nodiscard]] juce::ValueTree& getVariableTree() const { return m_variable_tree_state; }
        const juce::String& getProjectName() { return m_project_name; }

    private:
        juce::AudioProcessorValueTreeState& m_tree_state;
        juce::ValueTree& m_variable_tree_state;

        juce::String m_project_name {JUCE_PROJECT_NAME};
        juce::String m_folder_location {""};
    };
}