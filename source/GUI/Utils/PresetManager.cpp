//
// Created by Landon Viator on 12/1/25.
//

#include "PresetManager.h"

#include "Engine/Globals/Globals.h"

namespace viator::gui_utils
{
    PresetManager::PresetManager(juce::AudioProcessorValueTreeState& t, juce::ValueTree& v) : m_tree_state
    (t), m_variable_tree_state(v)
    {
        const auto name = viator::globals::TreeItems::presetFolder;
        const auto folderPath = m_variable_tree_state.getProperty(name);
        if (folderPath != "")
        {
            m_folder_location = folderPath.toString();
        } else {
            m_folder_location = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("ViatorDSP/" + m_project_name).getFullPathName();
        }
    }

    PresetManager::~PresetManager()
    {

    }

    void PresetManager::savePreset(const juce::File& fileToSave) const
    {
        juce::XmlElement xml(m_project_name);
        int paramCount = 0;

        for (auto* param : m_tree_state.processor.getParameters())
        {
            if (const auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                const juce::String paramID = rangedParam->getParameterID();

                if (const auto* value = m_tree_state.getRawParameterValue(paramID))
                {
                    auto* paramElement = xml.createNewChildElement("Parameter");
                    paramElement->setAttribute(paramID, value->load());
                    ++paramCount;
                }
            }
        }

        if (!xml.writeTo(fileToSave, {}))
            DBG("❌ Failed to write file.");
    }

    void PresetManager::loadPreset(const juce::String& filePath) const
    {
        const juce::File file(filePath);
        if (!file.existsAsFile())
            return;

        juce::String xmlData;

        if (auto inputStream = file.createInputStream())
            xmlData = inputStream->readEntireStreamAsString();
        else
            return;

        auto rootElement = juce::XmlDocument::parse(xmlData);
        if (rootElement == nullptr || !rootElement->hasTagName(m_project_name))
            return;

        auto* paramElement = rootElement->getFirstChildElement();

        while (paramElement != nullptr)
        {
            for (int i = 0; i < paramElement->getNumAttributes(); ++i)
            {
                const auto paramName = paramElement->getAttributeName(i);
                const auto paramValue = static_cast<float>(paramElement->getDoubleAttribute(paramName));

                if (auto* parameter = m_tree_state.getParameter(paramName))
                {
                    const auto normalised = parameter->convertTo0to1(paramValue);
                    parameter->setValueNotifyingHost(normalised);
                }
            }

            paramElement = paramElement->getNextElement();
        }
    }

    void PresetManager::createPresetFolder() const
    {
        juce::File folderPath = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("ViatorDSP/");

        const juce::File folder = folderPath.getChildFile(m_project_name);

        if (!folder.exists())
        {
            folder.createDirectory();
        }

        folderPath = folder.getFullPathName();
        const auto name = viator::globals::TreeItems::presetFolder;
        m_variable_tree_state.setProperty(name, folderPath.getFullPathName(), nullptr);

        juce::File defaultXmlFile = folderPath.getChildFile("Default.xml");

        if (!defaultXmlFile.existsAsFile())
        {
            savePreset(defaultXmlFile);
        }
    }
}
