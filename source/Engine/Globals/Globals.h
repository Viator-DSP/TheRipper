//
// Created by Landon Viator on 11/24/25.
//

#pragma once
#include <juce_audio_utils/juce_audio_utils.h>

namespace viator::globals
{
    struct Oversampling
    {
        static inline const juce::StringArray items = {"Off", "2X", "4X", "8X", "16X"};
    };

    struct MidSide
    {
        static inline const juce::StringArray items = {"Stereo", "Mid", "Sides"};
    };

    struct DistortionType
    {
        static inline const juce::StringArray items = {"Class B Amp", "Class A Valve", "Circle Map"};
    };

    struct ActionMessages
    {
        static inline const juce::String infoToggled = {"infoToggled"};
        static inline const juce::String presetAToggled = {"presetAToggled"};
        static inline const juce::String presetBToggled = {"presetBToggled"};
        static inline const juce::String refreshPresets = {"refreshPresets"};
        static inline const juce::String addItem = {"addItem"};
    };

    struct TreeItems
    {
        static inline const juce::String presetFolder = {"presetFolder"};
    };

    namespace PluginParameters
    {
        inline const juce::String gainID = "gainID";
        inline const juce::String gainName = "Input";

        inline const juce::String outputID = "outputID";
        inline const juce::String outputName = "Output";

        inline const juce::String oversamplingID = "oversamplingID";
        inline const juce::String oversamplingName = "HQ";

        inline const juce::String midSideID = "midSideID";
        inline const juce::String midSideName = "MidSide";

        inline const juce::String globalPowerID = "globalPowerID";
        inline const juce::String globalPowerName = "Power";

        inline const juce::String hpID = "hpID";
        inline const juce::String hpName = "HP";
        inline const juce::String lpID = "lpID";
        inline const juce::String lpName = "LP";
        inline const juce::String ripID = "ripID";
        inline const juce::String ripName = "Rip";
        inline const juce::String analogID = "analogID";
        inline const juce::String analogName = "Analog";
        inline const juce::String driveID = "driveID";
        inline const juce::String driveName = "Drive";
        inline const juce::String mixID = "mixID";
        inline const juce::String mixName = "Mix";
        inline const juce::String typeID = "typeID";
        inline const juce::String typeName = "Type";

        struct parameters
        {
            explicit parameters(const juce::AudioProcessorValueTreeState &tree)
            {
                gainParam = dynamic_cast<juce::AudioParameterFloat *>(
                    tree.getParameter(PluginParameters::gainID));
                outputParam = dynamic_cast<juce::AudioParameterFloat *>(
                    tree.getParameter(PluginParameters::outputID));

                oversamplingParam = dynamic_cast<juce::AudioParameterChoice *>(
                    tree.getParameter(PluginParameters::oversamplingID));
                midSideParam = dynamic_cast<juce::AudioParameterChoice *>(
                    tree.getParameter(PluginParameters::midSideID));
                powerParam = dynamic_cast<juce::AudioParameterBool *>(
                    tree.getParameter(PluginParameters::globalPowerID));

                hpParam = dynamic_cast<juce::AudioParameterFloat *>(
                        tree.getParameter(PluginParameters::hpID));
                lpParam = dynamic_cast<juce::AudioParameterFloat *>(
                        tree.getParameter(PluginParameters::lpID));

                ripParam = dynamic_cast<juce::AudioParameterBool *>(
                        tree.getParameter(PluginParameters::ripID));
                analogParam = dynamic_cast<juce::AudioParameterBool *>(
                        tree.getParameter(PluginParameters::analogID));

                driveParam = dynamic_cast<juce::AudioParameterFloat *>(
                        tree.getParameter(PluginParameters::driveID));
                mixParam = dynamic_cast<juce::AudioParameterFloat *>(
                        tree.getParameter(PluginParameters::mixID));

                typeParam = dynamic_cast<juce::AudioParameterChoice *>(
                        tree.getParameter(PluginParameters::typeID));
            }

            juce::AudioParameterFloat *gainParam{nullptr};
            juce::AudioParameterFloat *outputParam{nullptr};

            juce::AudioParameterChoice *oversamplingParam{nullptr};
            juce::AudioParameterChoice *midSideParam{nullptr};
            juce::AudioParameterBool *powerParam {nullptr};

            juce::AudioParameterFloat *hpParam{nullptr};
            juce::AudioParameterFloat *lpParam{nullptr};
            juce::AudioParameterBool *ripParam {nullptr};
            juce::AudioParameterBool *analogParam {nullptr};
            juce::AudioParameterFloat *driveParam{nullptr};
            juce::AudioParameterFloat *mixParam{nullptr};
            juce::AudioParameterChoice *typeParam{nullptr};

            std::vector<juce::String>& getControlParamIDs() { return m_param_ids; }
        private:
            std::vector<juce::String> m_param_ids {gainID, outputID, hpID, lpID, driveID, mixID, typeID};
        };
    }
}
