#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()    // mono input
                             .withInput("Input", juce::AudioChannelSet::stereo(), true)   // stereo input
                             .withOutput("Output", juce::AudioChannelSet::stereo(), true) // stereo output
      ), m_tree_state(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    m_parameters = std::make_unique<viator::globals::PluginParameters::parameters>(m_tree_state);

    for (const auto &param: m_parameters->getControlParamIDs()) {
        m_preset_a[param] = m_tree_state.getRawParameterValue(param)->load();
        m_preset_b[param] = m_tree_state.getRawParameterValue(param)->load();
    }
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;

    // OVERSAMPLING
    auto items = viator::globals::Oversampling::items;
    params.push_back(std::make_unique<juce::AudioParameterChoice>
    (juce::ParameterID{viator::globals::PluginParameters::oversamplingID, 1},
     viator::globals::PluginParameters::oversamplingName,
     items, 0));

    // MID SIDE
    items = viator::globals::MidSide::items;
    params.push_back(std::make_unique<juce::AudioParameterChoice>
    (juce::ParameterID{viator::globals::PluginParameters::midSideID, 1},
     viator::globals::PluginParameters::midSideName,
     items, 0));

    // POWER
    params.push_back(std::make_unique<juce::AudioParameterBool>
    (juce::ParameterID{viator::globals::PluginParameters::globalPowerID, 1},
     viator::globals::PluginParameters::globalPowerName, true));

    // BUTTONS
    params.push_back(std::make_unique<juce::AudioParameterBool>
                             (juce::ParameterID{viator::globals::PluginParameters::ripID, 1},
                              viator::globals::PluginParameters::ripName, false));
    params.push_back(std::make_unique<juce::AudioParameterBool>
                             (juce::ParameterID{viator::globals::PluginParameters::analogID, 1},
                              viator::globals::PluginParameters::analogID, false));

    params.push_back(std::make_unique<juce::AudioParameterInt>
                             (juce::ParameterID{viator::globals::PluginParameters::typeID, 1},
                              viator::globals::PluginParameters::typeName,
                              0,
                              viator::globals::DistortionType::items.size() - 1,
                              0));

    // SLIDERS
    params.push_back(std::make_unique<juce::AudioParameterFloat>
    (juce::ParameterID{viator::globals::PluginParameters::gainID, 1},
     viator::globals::PluginParameters::gainName, -20.0f, 20.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>
    (juce::ParameterID{viator::globals::PluginParameters::driveID, 1},
     viator::globals::PluginParameters::driveName, 0.0f, 30.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>
    (juce::ParameterID{viator::globals::PluginParameters::outputID, 1},
     viator::globals::PluginParameters::outputName, -20.0f, 20.0f, 0.0f));

    juce::NormalisableRange<float> hp_range = juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f);
    hp_range.setSkewForCentre(150.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                             (juce::ParameterID{viator::globals::PluginParameters::hpID, 1},
                              viator::globals::PluginParameters::hpName, hp_range, 20.0f));

    juce::NormalisableRange<float> lp_range = juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f);
    lp_range.setSkewForCentre(5000.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                             (juce::ParameterID{viator::globals::PluginParameters::lpID, 1},
                              viator::globals::PluginParameters::lpName, lp_range, 20000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>
                             (juce::ParameterID{viator::globals::PluginParameters::toneID, 1},
                              viator::globals::PluginParameters::toneName, -6.0f, 6.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>
                             (juce::ParameterID{viator::globals::PluginParameters::mixID, 1},
                              viator::globals::PluginParameters::mixName, 0.0f, 100.0f, 100.0f));

    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
}

void AudioPluginAudioProcessor::updateParameters()
{
    // OVERSAMPLING
    const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
    if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_processors.size()) {
        m_processors[static_cast<size_t>(oversampling_choice)].updateParameters(*m_parameters);
    }

    // MID SIDE
    const auto mid_side_choice = m_parameters->midSideParam->getIndex();
    if (mid_side_choice >= 0 && static_cast<size_t>(mid_side_choice) < viator::globals::MidSide::items.
        size()) {
        m_mid_side_processor.setMode(static_cast<viator::dsp::MidSideProcessor::StereoMode>(mid_side_choice));
    }

    const auto power = m_parameters->powerParam->get();
    for (auto &smoother: m_mute_smoothers) {
        smoother.setTargetValue(power);
    }
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    for (int i = 0; i < m_processors.size(); ++i) {
        m_processors[i].prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels(), i);
    }

    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    m_input_copy.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));

    m_mid_side_processor.prepare(spec);

    for (auto &smoother: m_mute_smoothers) {
        smoother.reset(sampleRate, 0.02);
        smoother.setCurrentAndTargetValue(1.0f);
    }

    for (auto &level: input_levels)
    {
        level.reset(sampleRate, 0.5);
    }

    for (auto &level: output_levels)
    {
        level.reset(sampleRate, 0.5);
    }

    m_distortion_smoothed.reset(sampleRate, 0.5);
    m_distortion_smoothed.setCurrentAndTargetValue(0.0f);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
           || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (getTotalNumInputChannels() == 1 && getTotalNumOutputChannels() == 2)
    {
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    }

    updateParameters();

    // POWER
    processPluginPower(buffer);

    // for (int channel = 0; channel < num_channels; ++channel)
    // {
    //     auto *data = buffer.getWritePointer(channel);
    //     for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    //     {
    //         data[sample] = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
    //     }
    // }

    m_input_copy.clear();
    m_input_copy.makeCopyOf(buffer, true);

    // INPUT
    buffer.applyGain(juce::Decibels::decibelsToGain(m_parameters->gainParam->get()));
    calculateInputPeakLevel(buffer);

    // MAIN PROCESSING
    if (buffer.getNumChannels() > 1)
    {
        m_mid_side_processor.process(buffer, [&](juce::AudioBuffer<float> &b, const int n)
    {
        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_processors.size())
        {
            m_processors[static_cast<size_t>(oversampling_choice)].process(b, n);
        }
    });
    } else
    {
        const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
        if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_processors.size())
        {
            m_processors[static_cast<size_t>(oversampling_choice)].process(buffer, buffer.getNumSamples());
        }
    }

    calculateDistortionDb(m_input_copy, buffer);

    const auto oversampling_choice = m_parameters->oversamplingParam->getIndex();
    if (oversampling_choice >= 0 && static_cast<size_t>(oversampling_choice) < m_processors.size()) {
        auto& distortion = m_processors[static_cast<size_t>(oversampling_choice)].getDistortion();
        buffer.applyGain(distortion.getDistortionCompensation());
    }

    // OUTPUT
    buffer.applyGain(juce::Decibels::decibelsToGain(m_parameters->outputParam->get()));
    calculateOutputPeakLevel(buffer);

    //buffer.applyGain(0.0f);
}

void AudioPluginAudioProcessor::processPluginPower(juce::AudioBuffer<float> &buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto *data = buffer.getWritePointer(channel);
        auto &gain = m_mute_smoothers[channel];

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            data[sample] *= gain.getNextValue();
        }
    }

    if (m_mute_smoothers[0].getCurrentValue() <= 0.0f)
        return;
}

void AudioPluginAudioProcessor::calculateInputPeakLevel(const juce::AudioBuffer<float> &buffer)
{
    const int numInputChannels = getTotalNumInputChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numInputChannels; ++ch)
    {
        input_levels[ch].skip(numSamples);
        input_peaks[ch] = buffer.getMagnitude(ch, 0, numSamples);

        if (input_peaks[ch] < input_levels[ch].getCurrentValue())
            input_levels[ch].setTargetValue(input_peaks[ch]);
        else
            input_levels[ch].setCurrentAndTargetValue(input_peaks[ch]);
    }

    for (int ch = numInputChannels; ch < 2; ++ch)
    {
        input_levels[ch].skip(numSamples);
        input_peaks[ch] = 0.0f;
        input_levels[ch].setTargetValue(0.0f);
    }
}

std::pair<float, float> AudioPluginAudioProcessor::getInputLevelsStereo() const
{
    return { input_levels[kLeft].getCurrentValue(), input_levels[kRight].getCurrentValue() };
}

std::pair<float, float> AudioPluginAudioProcessor::getOutputLevelsStereo() const
{
    return { output_levels[kLeft].getCurrentValue(), output_levels[kRight].getCurrentValue() };
}

void AudioPluginAudioProcessor::calculateOutputPeakLevel(const juce::AudioBuffer<float> &buffer)
{
    const int numInputChannels = getTotalNumInputChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numInputChannels; ++ch)
    {
        output_levels[ch].skip(numSamples);
        output_peaks[ch] = buffer.getMagnitude(ch, 0, numSamples);

        if (output_peaks[ch] < output_levels[ch].getCurrentValue())
            output_levels[ch].setTargetValue(output_peaks[ch]);
        else
            output_levels[ch].setCurrentAndTargetValue(output_peaks[ch]);
    }

    for (int ch = numInputChannels; ch < 2; ++ch)
    {
        output_levels[ch].skip(numSamples);
        output_peaks[ch] = 0.0f;
        output_levels[ch].setTargetValue(0.0f);
    }
}

void AudioPluginAudioProcessor::calculateDistortionDb(const juce::AudioBuffer<float>& inputBuffer,
                                                       const juce::AudioBuffer<float>& outputBuffer)
{
    const int numCh = juce::jmin(inputBuffer.getNumChannels(), outputBuffer.getNumChannels());
    const int nSamp = juce::jmin(inputBuffer.getNumSamples(), outputBuffer.getNumSamples());
    constexpr float distortion_calibration = 10.0f;

    if (numCh == 0 || nSamp == 0)
    {
        m_distortion_db.store(-100.0f);
        return;
    }

    double inputSumSq = 0.0;
    double residueSumSq = 0.0;

    for (int ch = 0; ch < numCh; ++ch)
    {
        const float* in = inputBuffer.getReadPointer(ch);
        const float* out = outputBuffer.getReadPointer(ch);

        for (int i = 0; i < nSamp; ++i)
        {
            const float input = in[i];
            const float residue = out[i] - input;
            inputSumSq   += input * input;
            residueSumSq += residue * residue;
        }
    }

    const float inputRMS = std::sqrt(inputSumSq / (numCh * nSamp));
    const float residueRMS = std::sqrt(residueSumSq / (numCh * nSamp));

    if (inputRMS < 1.0e-6f || residueRMS < 1.0e-6f)
    {
        m_distortion_db.store(-100.0f);
        return;
    }

    const float scaledResidue = residueRMS * distortion_calibration;
    const float db = juce::Decibels::gainToDecibels(scaledResidue, -100.0f);
    const float clamped = juce::jlimit(-20.0f, 0.0f, -db);
    m_distortion_smoothed.setTargetValue(clamped);
    m_distortion_smoothed.skip(nSamp);
}

float AudioPluginAudioProcessor::smoothDistortion(const float newValue)
{
    constexpr float attack = 0.05f;
    constexpr float release = 0.01f;

    const float current = m_distortion_db.load();
    const float coeff = (newValue > current) ? attack : release;

    const float smoothed = current + coeff * (newValue - current);
    m_distortion_db.store(smoothed);
    return smoothed;
}

float AudioPluginAudioProcessor::getDistortionDb() const
{
    return m_distortion_smoothed.getCurrentValue();
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    //return new AudioPluginAudioProcessorEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    getTreeState().state.appendChild(m_variable_tree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    getTreeState().state.writeToStream(stream);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    if (auto state = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes)); state.isValid()) {
        m_variable_tree = state.getChildWithName("Variables");
        getTreeState().state = state;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor * JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
