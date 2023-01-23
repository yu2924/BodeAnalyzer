/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.4

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "ProgressiveSessionDialog.h"
//[/Headers]

#include "StimulusForm.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace BAAPP {
//[/MiscUserDefs]

//==============================================================================
StimulusForm::StimulusForm (juce::AudioDeviceManager* adm, AnalysisController* ac)
    : audioDeviceManager(adm), analysisController(ac)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    titleLabel.reset (new juce::Label (juce::String(),
                                       TRANS("Stimulus")));
    addAndMakeVisible (titleLabel.get());
    titleLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    titleLabel->setJustificationType (juce::Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    titleLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    titleLabel->setBounds (8, 8, 104, 16);

    irPlotPane.reset (new Plot2dPane());
    addAndMakeVisible (irPlotPane.get());

    methodImpButton.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (methodImpButton.get());
    methodImpButton->setButtonText (TRANS("Impulse"));
    methodImpButton->setRadioGroupId (1);

    methodLinSSButton.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (methodLinSSButton.get());
    methodLinSSButton->setButtonText (TRANS("Linear SS"));
    methodLinSSButton->setRadioGroupId (1);

    methodLogSSButton.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (methodLogSSButton.get());
    methodLogSSButton->setButtonText (TRANS("Log SS"));
    methodLogSSButton->setRadioGroupId (1);

    amplitudeLabel.reset (new juce::Label (juce::String(),
                                           TRANS("Amplitude\n")));
    addAndMakeVisible (amplitudeLabel.get());
    amplitudeLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    amplitudeLabel->setJustificationType (juce::Justification::centredRight);
    amplitudeLabel->setEditable (false, false, false);
    amplitudeLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    amplitudeLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    amplitudeLabel->setBounds (8, 128, 80, 24);

    amplitudeEdit.reset (new juce::Label (juce::String(),
                                          juce::String()));
    addAndMakeVisible (amplitudeEdit.get());
    amplitudeEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    amplitudeEdit->setJustificationType (juce::Justification::centredRight);
    amplitudeEdit->setEditable (true, true, false);
    amplitudeEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    amplitudeEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    amplitudeEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    amplitudeEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colours::white);
    amplitudeEdit->addListener (this);

    amplitudeEdit->setBounds (88, 128, 64, 24);

    lengthLabel.reset (new juce::Label (juce::String(),
                                        TRANS("Length")));
    addAndMakeVisible (lengthLabel.get());
    lengthLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    lengthLabel->setJustificationType (juce::Justification::centredRight);
    lengthLabel->setEditable (false, false, false);
    lengthLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    lengthLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    lengthLabel->setBounds (160, 128, 64, 24);

    lengthCombo.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (lengthCombo.get());
    lengthCombo->setEditableText (false);
    lengthCombo->setJustificationType (juce::Justification::centredLeft);
    lengthCombo->setTextWhenNothingSelected (juce::String());
    lengthCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    lengthCombo->addListener (this);

    lengthCombo->setBounds (224, 128, 80, 24);

    repeatCountLabel.reset (new juce::Label (juce::String(),
                                             TRANS("Repeat")));
    addAndMakeVisible (repeatCountLabel.get());
    repeatCountLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    repeatCountLabel->setJustificationType (juce::Justification::centredRight);
    repeatCountLabel->setEditable (false, false, false);
    repeatCountLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    repeatCountLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    repeatCountLabel->setBounds (312, 128, 63, 24);

    repeatCountEdit.reset (new juce::Label (juce::String(),
                                            juce::String()));
    addAndMakeVisible (repeatCountEdit.get());
    repeatCountEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    repeatCountEdit->setJustificationType (juce::Justification::centredRight);
    repeatCountEdit->setEditable (true, true, false);
    repeatCountEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    repeatCountEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    repeatCountEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    repeatCountEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    repeatCountEdit->addListener (this);

    repeatCountEdit->setBounds (376, 128, 64, 24);

    startButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (startButton.get());
    startButton->setButtonText (TRANS("Start"));


    //[UserPreSize]
    irPlotPane->setAxes("Time (sec.)", { 0, 1 }, {}, false, nullptr,
                        "Amplitude", { -1, 1 }, { -1, 0, 1 }, false, nullptr);
    irPlotPane->adjustLabelBorder();
    methodImpButton->onClick = [this]() { onMethodButtonClick(); };
    methodLinSSButton->onClick = [this]() { onMethodButtonClick(); };
    methodLogSSButton->onClick = [this]() { onMethodButtonClick(); };
    amplitudeEdit->onTextChange = [this]() { onAmplitudeEditChnage(); };
    for(int order = 16; order <= 20; order++) lengthCombo->addItem(juce::String(1 << order), order);
    lengthCombo->onChange = [this]() { onLengthComboChange(); };
    repeatCountEdit->onTextChange = [this]() { onRepeatCountEditChnage(); };
    startButton->onClick = [this]() { onStartButtonClick(); };
    //[/UserPreSize]

    setSize (640, 160);


    //[Constructor] You can add your own custom stuff here..
    currentSampleRate = analysisController->getCurrentSampleRate();
    refrectProperties();
    audioDeviceManager->addChangeListener(this);
    analysisController->addChangeListener(this);
    analysisController->addProgressiveSessionListener(this);
    //[/Constructor]
}

StimulusForm::~StimulusForm()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    analysisController->removeProgressiveSessionListener(this);
    analysisController->removeChangeListener(this);
    audioDeviceManager->removeChangeListener(this);
    //[/Destructor_pre]

    titleLabel = nullptr;
    irPlotPane = nullptr;
    methodImpButton = nullptr;
    methodLinSSButton = nullptr;
    methodLogSSButton = nullptr;
    amplitudeLabel = nullptr;
    amplitudeEdit = nullptr;
    lengthLabel = nullptr;
    lengthCombo = nullptr;
    repeatCountLabel = nullptr;
    repeatCountEdit = nullptr;
    startButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void StimulusForm::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colours::white);

    //[UserPaint] Add your own custom painting code here..
    juce::Rectangle<float> rc = getLocalBounds().toFloat().reduced(4);
    g.setColour(juce::Colours::lightgrey);
    g.drawRoundedRectangle(rc, 3, 2);
    //[/UserPaint]
}

void StimulusForm::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    irPlotPane->setBounds (8, 24, getWidth() - 116, 104);
    methodImpButton->setBounds (getWidth() - 8 - 96, 24, 96, 24);
    methodLinSSButton->setBounds (getWidth() - 8 - 96, 48, 96, 24);
    methodLogSSButton->setBounds (getWidth() - 8 - 96, 72, 96, 24);
    startButton->setBounds (getWidth() - 8 - 96, 128, 96, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void StimulusForm::labelTextChanged (juce::Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == amplitudeEdit.get())
    {
        //[UserLabelCode_amplitudeEdit] -- add your label text handling code here..
        //[/UserLabelCode_amplitudeEdit]
    }
    else if (labelThatHasChanged == repeatCountEdit.get())
    {
        //[UserLabelCode_repeatCountEdit] -- add your label text handling code here..
        //[/UserLabelCode_repeatCountEdit]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void StimulusForm::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == lengthCombo.get())
    {
        //[UserComboBoxCode_lengthCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_lengthCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


// --------------------------------------------------------------------------------
// ChangeListener

void StimulusForm::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == audioDeviceManager)
    {
        if(currentSampleRate != analysisController->getCurrentSampleRate())
        {
            currentSampleRate = analysisController->getCurrentSampleRate();
            refrectProperties();
        }
    }
    else if(source == analysisController.get())
    {
        refrectProperties();
    }
}

// --------------------------------------------------------------------------------
 // ProgressiveSessionBase::Listener

void StimulusForm::progressiveSessionEnd(ProgressiveSessionBase* ps, const juce::Result& r, bool aborted)
{
    if(ps == analysisController.get())
    {
        if(r.failed() && !aborted)
        {
            juce::MessageManager::callAsync([this, r]() { juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", r.getErrorMessage()); });
        }
    }
}

// --------------------------------------------------------------------------------
// internal

void StimulusForm::refrectProperties()
{
    double fs = analysisController->getCurrentSampleRate();
    const AnalysisController::Parameters& params = analysisController->getParameters();
    // plot
    const std::vector<float>& stimulus = analysisController->getStimulus();
    auto ds = Plot2dPane::createFloatArrayDataSource(0, stimulus.data(), stimulus.size(), false, [fs](size_t i, float v)->Plot2dPane::Point { return { (float)(i / fs), v }; });
    irPlotPane->clearDataSources();
    irPlotPane->addDataSource(ds);
    irPlotPane->adjustRangesToFit(true, false);
    // methods
    methodImpButton->setToggleState(params.method == AnalysisController::MethodImpulse, juce::dontSendNotification);
    methodLinSSButton->setToggleState(params.method == AnalysisController::MethodLinearSS, juce::dontSendNotification);
    methodLogSSButton->setToggleState(params.method == AnalysisController::MethodLogSS, juce::dontSendNotification);
    // amplitude
    amplitudeEdit->setText(juce::String(params.amplitude), juce::dontSendNotification);
    // length
    lengthCombo->setSelectedId(params.order, juce::dontSendNotification);
    // repeatCount
    repeatCountEdit->setText(juce::String(params.repeatCount), juce::dontSendNotification);
}

void StimulusForm::onMethodButtonClick()
{
    irPlotPane->clearDataSources();
    auto params = analysisController->getParameters();
    if     (methodImpButton->getToggleState()) params.method = AnalysisController::MethodImpulse;
    else if(methodLinSSButton->getToggleState()) params.method = AnalysisController::MethodLinearSS;
    else if(methodLogSSButton->getToggleState()) params.method = AnalysisController::MethodLogSS;
    analysisController->setParameters(params);
}

void StimulusForm::onAmplitudeEditChnage()
{
    irPlotPane->clearDataSources();
    auto params = analysisController->getParameters();
    params.amplitude = amplitudeEdit->getText().getFloatValue();
    analysisController->setParameters(params);
}

void StimulusForm::onLengthComboChange()
{
    irPlotPane->clearDataSources();
    auto params = analysisController->getParameters();
    params.order = lengthCombo->getSelectedId();
    analysisController->setParameters(params);
}

void StimulusForm::onRepeatCountEditChnage()
{
    irPlotPane->clearDataSources();
    auto params = analysisController->getParameters();
    params.repeatCount = repeatCountEdit->getText().getIntValue();
    analysisController->setParameters(params);
}

void StimulusForm::onStartButtonClick()
{
    juce::Result r = analysisController->startSession();
    if(r.wasOk()) progressiveSessionDialogShowAsync(analysisController.get(), "now measuring...");
    else juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", r.getErrorMessage());
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="StimulusForm" componentName=""
                 parentClasses="public juce::Component, public juce::ChangeListener, public ProgressiveSessionBase::Listener"
                 constructorParams="juce::AudioDeviceManager* adm, AnalysisController* ac"
                 variableInitialisers="audioDeviceManager(adm), analysisController(ac)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="640" initialHeight="160">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="" id="cfb76c1374e76240" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 104 16" edTextCol="ff000000"
         edBkgCol="0" labelText="Stimulus" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="1" italic="0" justification="33" typefaceStyle="Bold"/>
  <GENERICCOMPONENT name="" id="83ee5810795a7a2" memberName="irPlotPane" virtualName=""
                    explicitFocusOrder="0" pos="8 24 116M 104" class="Plot2dPane"
                    params=""/>
  <TOGGLEBUTTON name="" id="7110fa476e68cdd" memberName="methodImpButton" virtualName=""
                explicitFocusOrder="0" pos="8Rr 24 96 24" buttonText="Impulse"
                connectedEdges="0" needsCallback="0" radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="" id="ace919d0b87fc543" memberName="methodLinSSButton"
                virtualName="" explicitFocusOrder="0" pos="8Rr 48 96 24" buttonText="Linear SS"
                connectedEdges="0" needsCallback="0" radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="" id="8ad28d2b343dbe2a" memberName="methodLogSSButton"
                virtualName="" explicitFocusOrder="0" pos="8Rr 72 96 24" buttonText="Log SS"
                connectedEdges="0" needsCallback="0" radioGroupId="1" state="0"/>
  <LABEL name="" id="117a0a7778f7a676" memberName="amplitudeLabel" virtualName=""
         explicitFocusOrder="0" pos="8 128 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Amplitude&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="6abc02acefd85ab5" memberName="amplitudeEdit" virtualName=""
         explicitFocusOrder="0" pos="88 128 64 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="ffffffff" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="5c69a5c5bc76c089" memberName="lengthLabel" virtualName=""
         explicitFocusOrder="0" pos="160 128 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Length" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="" id="f48bc0851be48abb" memberName="lengthCombo" virtualName=""
            explicitFocusOrder="0" pos="224 128 80 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="665de043b8f2a224" memberName="repeatCountLabel" virtualName=""
         explicitFocusOrder="0" pos="312 128 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Repeat" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="f58a9627fbca840c" memberName="repeatCountEdit" virtualName=""
         explicitFocusOrder="0" pos="376 128 64 24" bkgCol="ffffffff"
         outlineCol="ffdddddd" edTextCol="ff000000" edBkgCol="0" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="34"/>
  <TEXTBUTTON name="" id="bd896244a95b0ca4" memberName="startButton" virtualName=""
              explicitFocusOrder="0" pos="8Rr 128 96 24" buttonText="Start"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
} // namespace BAAPP
//[/EndFile]

