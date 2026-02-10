#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p),
      m_header(processorRef.getTreeState(), processorRef.getVariableTree())
{
    juce::ignoreUnused(processorRef);

    addAndMakeVisible(m_header);

    initSliders();
    initMainSliders();
    initButtons();

    initMeters();

    addAndMakeVisible(m_vu_meter);

    addAndMakeVisible(m_info_panel);

    m_info_panel.setVisible(false);
    m_header.addActionListener(this);

    startTimerHz(30.0f);

    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(2.5);
    setResizeLimits(625, 250, 1500, 600);
    setSize(1000, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();

    m_header.removeActionListener(this);

    m_sliders[kLeft].onValueChange = nullptr;
    m_sliders[kRight].onValueChange = nullptr;

    m_gain_attach.reset();
    m_volume_attach.reset();
    m_drive_attach.reset();
    m_mix_attach.reset();
    m_type_attach.reset();
    m_tone_attach.reset();
    m_hp_attach.reset();
    m_lp_attach.reset();
    m_rip_attach.reset();
    m_analog_attach.reset();

    for (auto &slider: m_sliders) slider.setLookAndFeel(nullptr);
    for (auto &slider: m_main_sliders) slider.setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    const auto bg_color = viator::gui_utils::Colors::editor_bg_color();
    g.setColour(bg_color);
    g.fillRect(getLocalBounds());

    const auto line_color = viator::gui_utils::Colors::widget_off().darker(0.6f);
    constexpr auto padding = 10.0f;
    g.setColour(line_color);
    g.drawRect(
        getLocalBounds().toFloat().withSizeKeepingCentre(static_cast<float>(getWidth()) - padding,
                                                         static_cast<float>(getHeight()) * 0.75f),
        2.0f);

    const auto inner_bounds = getLocalBounds().toFloat().withSizeKeepingCentre(
        static_cast<float>(getWidth()) - padding * 2,
        static_cast<float>(getHeight()) * 0.73f - padding);
    const auto &texture = viator::gui_utils::Images::texture();
    g.drawImage(texture, inner_bounds, juce::RectanglePlacement::stretchToFit);
    g.setColour(juce::Colour(30, 62, 98).darker(1.0f).withAlpha(0.8f));
    g.fillRect(inner_bounds);

    placeScrews(g);

    // // place icons here
    // const auto slider_size = m_main_sliders[kType].getWidth();
    //
    // // BIT ICON
    // if (auto *d = viator::gui_utils::Images::bit_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getRight() - slider_size / 4)
    //                   .withY(m_main_sliders[kType].getBottom() - slider_size / 4)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
    //
    // // AMP ICON
    // if (auto *d = viator::gui_utils::Images::amp_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getX() + 10)
    //                   .withY(m_main_sliders[kType].getBottom() - slider_size / 4)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
    //
    // // TUBE ICON
    // if (auto *d = viator::gui_utils::Images::tube_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getX())
    //                   .withY(m_main_sliders[kType].getBottom() - slider_size * 0.63)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
    //
    //
    // // CIRCLE ICON
    // if (auto *d = viator::gui_utils::Images::circle_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getX() + slider_size * 0.21f)
    //                   .withY(m_main_sliders[kType].getY() + slider_size * 0.036f)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
    //
    // // TAPE ICON
    // if (auto *d = viator::gui_utils::Images::tape_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getX() + slider_size * 0.64f)
    //                   .withY(m_main_sliders[kType].getY() + slider_size * 0.036f)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
    //
    // // OVERDRIVE ICON
    // if (auto *d = viator::gui_utils::Images::overdrive_icon())
    //     d->drawWithin(g, getLocalBounds().toFloat()
    //                   .withX(m_main_sliders[kType].getX() + slider_size * 0.88f)
    //                   .withY(m_main_sliders[kType].getY() + slider_size * 0.37f)
    //                   .withWidth(16).withHeight(16), juce::RectanglePlacement::stretchToFit, 1.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    m_header.setBounds(0, 0, getWidth(), juce::roundToInt(getHeight() * 0.12));
    m_info_panel.setBounds(getLocalBounds().withSizeKeepingCentre(
        juce::roundToInt(getWidth() * 0.9),
        juce::roundToInt(getHeight() * 0.7)));

    auto dial_size = juce::roundToInt(getHeight() * 0.09);
    auto dial_y = getHeight() - juce::roundToInt(dial_size * 1.025);
    m_sliders[kLeft].setBounds(dial_size * 2, dial_y, dial_size, dial_size);
    m_sliders[kRight].setBounds(getWidth() - dial_size * 3, dial_y, dial_size, dial_size);

    m_input_meter[kLeft].setBounds(m_sliders[kLeft].getRight() + dial_size / 4, dial_y, dial_size / 5,
                                   juce::roundToInt(dial_size * 0.95));
    m_input_meter[kRight].setBounds(m_input_meter[kLeft].getRight(), dial_y, dial_size / 5,
                                    juce::roundToInt(dial_size * 0.95));
    m_output_meter[kLeft].setBounds(m_sliders[kRight].getX() - dial_size, dial_y, dial_size / 5,
                                    juce::roundToInt(dial_size * 0.95));
    m_output_meter[kRight].setBounds(m_output_meter[kLeft].getRight(), dial_y, dial_size / 5,
                                     juce::roundToInt(dial_size * 0.95));

    m_slider_popup_labels[kLeft].setBounds(0, dial_y, dial_size * 2, dial_size);
    m_slider_popup_labels[kRight].setBounds(m_sliders[kRight].getRight(), dial_y, dial_size * 2, dial_size);

    const auto font_size = juce::jmax(static_cast<float>(getHeight()) * 0.04f, 10.0f);
    m_slider_popup_labels[kLeft].setFont(viator::gui_utils::Fonts::regular(font_size));
    m_slider_popup_labels[kRight].setFont(viator::gui_utils::Fonts::regular(font_size));

    // MAIN PLUGIN
    constexpr auto vu_size_scalar = 1.15;
    const auto vu_width = juce::roundToInt(getWidth() * 0.2 * vu_size_scalar);
    const auto vu_height = juce::roundToInt(vu_width * 0.647);
    const auto vu_y = juce::roundToInt(getHeight() * 0.14);
    m_vu_meter.setBounds(getLocalBounds().withSizeKeepingCentre(vu_width, vu_height).withY(vu_y));

    auto dial_x = fromW(0.013);
    dial_y = fromH(0.25);
    dial_size = fromW(0.13);
    m_main_sliders[kDrive].setBounds(dial_x, dial_y, dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kDrive], m_main_slider_popup_labels[kDrive], font_size);
    m_rip_button.setBounds(juce::roundToInt(getWidth() * 0.112),
                           m_main_sliders[kDrive].getBottom() + dial_size / 4, dial_size / 2,
                           juce::roundToInt(dial_size * 0.423));

    dial_x += dial_size;
    m_main_sliders[kMix].setBounds(dial_x, dial_y, dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kMix], m_main_slider_popup_labels[kMix], font_size);

    dial_x += dial_size;
    m_main_sliders[kType].setBounds(dial_x, dial_y + dial_size * 0.65, dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kType], m_main_slider_popup_labels[kType], font_size);

    dial_x = fromW(0.572);
    m_main_sliders[kTone].setBounds(dial_x, m_main_sliders[kType].getY(), dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kTone], m_main_slider_popup_labels[kTone], font_size);

    dial_x += dial_size;

    m_main_sliders[kHP].setBounds(dial_x, dial_y, dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kHP], m_main_slider_popup_labels[kHP], font_size);
    m_analog_button.setBounds(juce::roundToInt(getWidth() * 0.828),
                              m_main_sliders[kHP].getBottom() + dial_size / 4, dial_size / 2,
                              juce::roundToInt(dial_size * 0.423));


    dial_x += dial_size;

    m_main_sliders[kLP].setBounds(dial_x, dial_y, dial_size, dial_size);
    positionLabelForDial(m_main_sliders[kLP], m_main_slider_popup_labels[kLP], font_size);
}

void AudioPluginAudioProcessorEditor::drawIconCopy(juce::Graphics &g,
                                                   const juce::Drawable *icon,
                                                   const juce::Rectangle<float> &bounds,
                                                   std::initializer_list<std::pair<juce::Colour, juce::Colour> >
                                                   colorReplacements,
                                                   const float opacity)
{
    if (!icon)
        return;

    if (const auto drawable = icon->createCopy()) {
        for (const auto &[src, dst]: colorReplacements)
            drawable->replaceColour(src, dst);

        drawable->drawWithin(g, bounds,
                             juce::RectanglePlacement::stretchToFit,
                             opacity);
    }
}

void AudioPluginAudioProcessorEditor::initSliders()
{
    for (auto &slider: m_sliders) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::ColourIds::thumbColourId, viator::gui_utils::Colors::text());
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(234, 234, 234));
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         viator::gui_utils::Colors::widget_center().brighter(0.15f));
        slider.setLookAndFeel(&m_thin_dial_laf);
        addAndMakeVisible(slider);
    }

    m_sliders[kLeft].setComponentID(viator::globals::PluginParameters::gainID);
    m_sliders[kRight].setComponentID(viator::globals::PluginParameters::outputID);

    m_sliders[kLeft].setName(viator::globals::PluginParameters::gainName);
    m_sliders[kRight].setName(viator::globals::PluginParameters::outputName);

    m_gain_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::gainID,
        m_sliders[kLeft]);
    m_volume_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::outputID,
        m_sliders[kRight]);

    m_sliders[kLeft].onValueChange = [this]()
    {
        save_preset(m_sliders[kLeft].getComponentID(), static_cast<float>(m_sliders[kLeft].getValue()));
    };

    m_sliders[kRight].onValueChange = [this]()
    {
        save_preset(m_sliders[kRight].getComponentID(), static_cast<float>(m_sliders[kRight].getValue()));
    };

    for (auto &label: m_slider_popup_labels) {
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }
}

void AudioPluginAudioProcessorEditor::initMainSliders()
{
    for (int i = 0; i < m_main_sliders.size(); i++) {
        setSliderProps(m_main_sliders[static_cast<MainSliders>(i)]);
        m_main_sliders[i].setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
    }

    m_main_sliders[kDrive].setName("Drive");
    m_main_sliders[kDrive].setTextValueSuffix("dB");

    m_main_sliders[kMix].setName("Mix");
    m_main_sliders[kMix].setTextValueSuffix("%");

    m_main_sliders[kType].setName("Type");
    m_main_sliders[kType].setTextValueSuffix("");
    m_main_sliders[kType].setKnobType(viator::gui::widgets::BaseKnob::KnobType::kChicken);

    m_main_sliders[kHP].setName("HP");
    m_main_sliders[kHP].setTextValueSuffix("Hz");
    m_main_sliders[kHP].setKnobType(viator::gui::widgets::BaseKnob::KnobType::kSynth);

    m_main_sliders[kTone].setName("Tone");
    m_main_sliders[kTone].setTextValueSuffix("dB");
    m_main_sliders[kTone].setKnobType(viator::gui::widgets::BaseKnob::KnobType::kChicken);

    m_main_sliders[kLP].setName("LP");
    m_main_sliders[kLP].setTextValueSuffix("Hz");
    m_main_sliders[kLP].setKnobType(viator::gui::widgets::BaseKnob::KnobType::kSynth);

    m_drive_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::driveID,
        m_main_sliders[kDrive]);
    m_mix_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::mixID,
        m_main_sliders[kMix]);
    m_type_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::typeID,
        m_main_sliders[kType]);
    m_tone_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::toneID,
        m_main_sliders[kTone]);
    m_hp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::hpID,
        m_main_sliders[kHP]);
    m_lp_attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef
        .getTreeState(),
        viator::globals::PluginParameters::lpID,
        m_main_sliders[kLP]);

    for (auto &label: m_main_slider_popup_labels) {
        label.setJustificationType(juce::Justification::centred);
        //label.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }
}

void AudioPluginAudioProcessorEditor::initButtons()
{
    addAndMakeVisible(m_rip_button);
    addAndMakeVisible(m_analog_button);

    m_rip_button.setButtonText("RIP");
    m_analog_button.setButtonText("ANALOG");

    m_rip_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getTreeState(), viator::globals::PluginParameters::ripID, m_rip_button);
    m_analog_attach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getTreeState(), viator::globals::PluginParameters::analogID, m_analog_button);
}

void AudioPluginAudioProcessorEditor::actionListenerCallback(const juce::String &message)
{
    if (message == viator::globals::ActionMessages::infoToggled) {
        m_info_panel.setVisible(!m_info_panel.isVisible());
    } else if (message == viator::globals::ActionMessages::presetAToggled) {
        apply_preset(true);
    } else if (message == viator::globals::ActionMessages::presetBToggled) {
        apply_preset(false);
    }
}

void AudioPluginAudioProcessorEditor::apply_preset(const bool isPresetA) const
{
    const auto &preset_map = isPresetA ? processorRef.getPresetA() : processorRef.getPresetB();
    const auto &tree = processorRef.getTreeState();

    for (const auto &item: preset_map) {
        const auto &parameter = tree.getParameter(item.first);
        const auto normalizedValue = parameter->convertTo0to1(item.second);

        if (item.first == viator::globals::PluginParameters::oversamplingID) {
            //hq_menu.setSelectedId(juce::roundToInt(item.second));
        } else if (item.first == viator::globals::PluginParameters::midSideID) {
            //stereo_menu.setSelectedId(juce::roundToInt(item.second));
        } else {
            parameter->setValueNotifyingHost(normalizedValue);
        }
    }
}

void AudioPluginAudioProcessorEditor::save_preset(const juce::String &param, const float paramValue)
{
    // auto &preset_map = m_header.getPresetAButton().getToggleState()
    //                        ? processorRef.getPresetA()
    //                        : processorRef
    //                        .getPresetB();
    //preset_map[param] = paramValue;
}

void AudioPluginAudioProcessorEditor::initMeters()
{
    for (auto &meter: m_input_meter) {
        addAndMakeVisible(meter);
    }
    for (auto &meter: m_output_meter) {
        addAndMakeVisible(meter);
    }
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    if (const auto safeThis = juce::Component::SafePointer<AudioPluginAudioProcessorEditor>(this); !safeThis)
        return;

    const auto in = processorRef.getInputLevelsStereo();
    const auto out = processorRef.getOutputLevelsStereo();
    const auto drive = processorRef.getDistortionDb();

    m_input_meter[kLeft].setLevel(in.first);
    m_output_meter[kLeft].setLevel(out.first);
    m_input_meter[kRight].setLevel(in.second);
    m_output_meter[kRight].setLevel(out.second);

    m_vu_meter.setLevel(drive);

    // HOVER
    for (int i = 0; i < m_sliders.size(); ++i) {
        const auto is_over = m_sliders[i].isMouseOverOrDragging();
        const auto value = m_sliders[i].getValue();
        const auto name = m_sliders[i].getName();
        m_slider_popup_labels[i].setText(is_over ? juce::String(value, 2) : name, juce::dontSendNotification);
    }

    for (int i = 0; i < m_main_sliders.size(); ++i) {
        const auto is_over = m_main_sliders[i].isMouseOverOrDragging();
        const auto value = m_main_sliders[i].getValue();
        const auto name = m_main_sliders[i].getName();
        m_main_slider_popup_labels[i].setText(is_over
                                                  ? juce::String(value, 2) + " " +
                                                    m_main_sliders[i].getTextValueSuffix()
                                                  : name, juce::dontSendNotification);
    }

    const auto is_over = m_main_sliders[kType].isMouseOverOrDragging();
    const auto value = m_main_sliders[kType].getValue();
    const auto name = m_main_sliders[kType].getName();
    const auto &type_string = viator::globals::DistortionType::items[static_cast<int>(value)];
    m_main_slider_popup_labels[kType].setText(is_over ? type_string : name, juce::dontSendNotification);
}

void AudioPluginAudioProcessorEditor::setSliderProps(juce::Slider &slider)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setLookAndFeel(&m_thin_dial_laf);
    addAndMakeVisible(slider);
}

void AudioPluginAudioProcessorEditor::positionLabelForDial(juce::Slider &slider, juce::Label &label,
                                                           const float font_size)
{
    label.setBounds(slider.getX(), slider.getBottom(),
                    slider.getWidth(), slider.getHeight() / 5);
    label.setFont(viator::gui_utils::Fonts::regular(font_size));
}

int AudioPluginAudioProcessorEditor::fromW(const double mult) const
{
    return juce::roundToInt(getWidth() * mult);
}

int AudioPluginAudioProcessorEditor::fromH(const double mult) const
{
    return juce::roundToInt(getHeight() * mult);
}

void AudioPluginAudioProcessorEditor::placeScrews(const juce::Graphics &g) const
{
    const auto screw = viator::gui_utils::Images::screw();

    const auto w = static_cast<float>(getWidth());
    const auto h = static_cast<float>(getHeight());

    const auto screwW = w / 40.0f;
    const auto screwH = screwW * 1.098f;

    const auto insetX = w * 0.01f;
    const auto topY = h * 0.12f + screwH * 0.5f;
    const auto botY = h * 0.88f - screwH * 1.5f;

    const auto leftX = insetX;
    const auto rightX = w - insetX - screwW;

    const std::array<juce::Point<float>, 4> p{
        juce::Point<float>(leftX, topY),
        juce::Point<float>(rightX, topY),
        juce::Point<float>(rightX, botY),
        juce::Point<float>(leftX, botY)
    };

    for (const auto pt: p) {
        g.drawImageWithin(screw,
                          juce::roundToInt(pt.x), juce::roundToInt(pt.y),
                          juce::roundToInt(screwW), juce::roundToInt(screwH),
                          juce::RectanglePlacement::stretchToFit);
    }
}
