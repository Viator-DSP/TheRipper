#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Engine/Globals/Globals.h"
#include "DSP/ProcessBlock.h"
#include "DSP/Modules/MidSideProcessor.h"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState& getTreeState() { return m_tree_state; }
    juce::ValueTree& getVariableTree() { return m_variable_tree; }

    std::map<juce::String, float>& getPresetA() { return m_preset_a; }
    std::map<juce::String, float>& getPresetB() { return m_preset_b; }

    std::pair<float, float> getInputLevelsStereo() const;
    std::pair<float, float> getOutputLevelsStereo() const;

    float getDistortionDb() const;

    enum Channels
    {
        kLeft = 0,
        kRight,
        num_channels
    };
private:
    //==============================================================================

    juce::AudioProcessorValueTreeState m_tree_state;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String &parameterID, float newValue) override;

    juce::ValueTree m_variable_tree {
        "Variables", {}, {
                        { "Group", {{"name", "Vars"}} },
                        { "Parameter", {{"id", "width"}, {"value", 1230.0}} },
                        { "Parameter", {{"id", "height"}, {"value", 730.0}} },
                        { "Parameter", {{"id", "presetFolder"}, {"value", ""}} },
                        { "Parameter", {{"id", "stereoMenu"}, {"value", 0}} },
                        { "Parameter", {{"id", "hqMenu"}, {"value", 1}} },
                        { "Parameter", {{"id", "sizeChoice"}, {"value", 3}} }
        }
    };

    void updateParameters();
    void processPluginPower(juce::AudioBuffer<float>& buffer);
    void calculateInputPeakLevel(const juce::AudioBuffer<float> &buffer);
    void calculateOutputPeakLevel(const juce::AudioBuffer<float> &buffer);

    std::unique_ptr<viator::globals::PluginParameters::parameters> m_parameters;
    std::array<viator::dsp::ProcessBlock, 5> m_processors;
    viator::dsp::MidSideProcessor m_mid_side_processor;
    std::array<juce::SmoothedValue<float>, num_channels> m_mute_smoothers;
    std::map<juce::String, float> m_preset_a, m_preset_b;
    std::array<juce::SmoothedValue<float>, num_channels> input_levels;
    std::array<float, num_channels> input_peaks;
    std::array<juce::SmoothedValue<float>, num_channels> output_levels;
    std::array<float, num_channels> output_peaks;

    std::atomic<float> m_distortion_db { -100.0f };
    void calculateDistortionDb(const juce::AudioBuffer<float>& inputBuffer,
                                                       const juce::AudioBuffer<float>& outputBuffer);
    float smoothDistortion(float newValue);
    juce::AudioBuffer<float> m_input_copy;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> m_distortion_smoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
