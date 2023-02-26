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

#include "STStimulusForm.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace BAAPP {
namespace ST {
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

    titleLabel->setBounds (8, 8, 96, 16);

    freqRangeLabel.reset (new juce::Label (juce::String(),
                                           TRANS("Frequency Range")));
    addAndMakeVisible (freqRangeLabel.get());
    freqRangeLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    freqRangeLabel->setJustificationType (juce::Justification::centredLeft);
    freqRangeLabel->setEditable (false, false, false);
    freqRangeLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freqRangeLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freqRangeLabel->setBounds (16, 32, 128, 24);

    freqMinLabel.reset (new juce::Label (juce::String(),
                                         TRANS("min.")));
    addAndMakeVisible (freqMinLabel.get());
    freqMinLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    freqMinLabel->setJustificationType (juce::Justification::centredRight);
    freqMinLabel->setEditable (false, false, false);
    freqMinLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freqMinLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freqMinLabel->setBounds (144, 32, 40, 24);

    freqMinEdit.reset (new juce::Label (juce::String(),
                                        juce::String()));
    addAndMakeVisible (freqMinEdit.get());
    freqMinEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    freqMinEdit->setJustificationType (juce::Justification::centredRight);
    freqMinEdit->setEditable (true, true, false);
    freqMinEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    freqMinEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    freqMinEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freqMinEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    freqMinEdit->addListener (this);

    freqMinEdit->setBounds (184, 32, 80, 24);

    freqMaxLabel.reset (new juce::Label (juce::String(),
                                         TRANS("max.")));
    addAndMakeVisible (freqMaxLabel.get());
    freqMaxLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    freqMaxLabel->setJustificationType (juce::Justification::centredRight);
    freqMaxLabel->setEditable (false, false, false);
    freqMaxLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freqMaxLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freqMaxLabel->setBounds (272, 32, 40, 24);

    freqMaxEdit.reset (new juce::Label (juce::String(),
                                        juce::String()));
    addAndMakeVisible (freqMaxEdit.get());
    freqMaxEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    freqMaxEdit->setJustificationType (juce::Justification::centredRight);
    freqMaxEdit->setEditable (true, true, false);
    freqMaxEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    freqMaxEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    freqMaxEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freqMaxEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    freqMaxEdit->addListener (this);

    freqMaxEdit->setBounds (312, 32, 80, 24);

    numPointsLabel.reset (new juce::Label (juce::String(),
                                           TRANS("Measuring Points")));
    addAndMakeVisible (numPointsLabel.get());
    numPointsLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    numPointsLabel->setJustificationType (juce::Justification::centredLeft);
    numPointsLabel->setEditable (false, false, false);
    numPointsLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    numPointsLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    numPointsLabel->setBounds (16, 64, 128, 24);

    numPointsEdit.reset (new juce::Label (juce::String(),
                                          juce::String()));
    addAndMakeVisible (numPointsEdit.get());
    numPointsEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    numPointsEdit->setJustificationType (juce::Justification::centredRight);
    numPointsEdit->setEditable (true, true, false);
    numPointsEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    numPointsEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    numPointsEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    numPointsEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    numPointsEdit->addListener (this);

    numPointsEdit->setBounds (184, 64, 80, 24);

    logCheck.reset (new juce::ToggleButton (juce::String()));
    addAndMakeVisible (logCheck.get());
    logCheck->setButtonText (TRANS("Logarithmic"));

    logCheck->setBounds (272, 64, 120, 24);

    amplitudeLabel.reset (new juce::Label (juce::String(),
                                           TRANS("Amplitude")));
    addAndMakeVisible (amplitudeLabel.get());
    amplitudeLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    amplitudeLabel->setJustificationType (juce::Justification::centredLeft);
    amplitudeLabel->setEditable (false, false, false);
    amplitudeLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    amplitudeLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    amplitudeLabel->setBounds (16, 96, 128, 24);

    amplitudeEdit.reset (new juce::Label (juce::String(),
                                          juce::String()));
    addAndMakeVisible (amplitudeEdit.get());
    amplitudeEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    amplitudeEdit->setJustificationType (juce::Justification::centredRight);
    amplitudeEdit->setEditable (true, true, false);
    amplitudeEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    amplitudeEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    amplitudeEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    amplitudeEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    amplitudeEdit->addListener (this);

    amplitudeEdit->setBounds (184, 96, 80, 24);

    repeatCountLabel.reset (new juce::Label (juce::String(),
                                             TRANS("Repeat Count")));
    addAndMakeVisible (repeatCountLabel.get());
    repeatCountLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    repeatCountLabel->setJustificationType (juce::Justification::centredLeft);
    repeatCountLabel->setEditable (false, false, false);
    repeatCountLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    repeatCountLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    repeatCountLabel->setBounds (16, 128, 128, 24);

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

    repeatCountEdit->setBounds (184, 128, 80, 24);

    startButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (startButton.get());
    startButton->setButtonText (TRANS("Start"));


    //[UserPreSize]
    freqMinEdit->onTextChange = [this]() { onFreqRangeEditChange(); };
    freqMaxEdit->onTextChange = [this]() { onFreqRangeEditChange(); };
    numPointsEdit->onTextChange = [this]() { onNumPointsEditChange(); };
    amplitudeEdit->onTextChange = [this]() { onAmplitudeEditChange(); };
    repeatCountEdit->onTextChange = [this]() { onRepeatCountEditChange(); };
    logCheck->onClick = [this]() { onLogCheckClick(); };
    startButton->onClick = [this]() { onStartButtonClick(); };
    //[/UserPreSize]

    setSize (640, 168);


    //[Constructor] You can add your own custom stuff here..
    refrectProperties();
    analysisController->addChangeListener(this);
    analysisController->addProgressiveSessionListener(this);
    //[/Constructor]
}

StimulusForm::~StimulusForm()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    analysisController->removeProgressiveSessionListener(this);
    analysisController->removeChangeListener(this);
    //[/Destructor_pre]

    titleLabel = nullptr;
    freqRangeLabel = nullptr;
    freqMinLabel = nullptr;
    freqMinEdit = nullptr;
    freqMaxLabel = nullptr;
    freqMaxEdit = nullptr;
    numPointsLabel = nullptr;
    numPointsEdit = nullptr;
    logCheck = nullptr;
    amplitudeLabel = nullptr;
    amplitudeEdit = nullptr;
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

    startButton->setBounds (getWidth() - 16 - 96, 128, 96, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void StimulusForm::labelTextChanged (juce::Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == freqMinEdit.get())
    {
        //[UserLabelCode_freqMinEdit] -- add your label text handling code here..
        //[/UserLabelCode_freqMinEdit]
    }
    else if (labelThatHasChanged == freqMaxEdit.get())
    {
        //[UserLabelCode_freqMaxEdit] -- add your label text handling code here..
        //[/UserLabelCode_freqMaxEdit]
    }
    else if (labelThatHasChanged == numPointsEdit.get())
    {
        //[UserLabelCode_numPointsEdit] -- add your label text handling code here..
        //[/UserLabelCode_numPointsEdit]
    }
    else if (labelThatHasChanged == amplitudeEdit.get())
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



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

// ChangeListener

void StimulusForm::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == analysisController.get())
    {
        refrectProperties();
    }
}

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

// internal

void StimulusForm::refrectProperties()
{
    const AnalysisController::Parameters& params = analysisController->getParameters();
    freqMinEdit->setText(juce::String(params.freqMinHz), juce::dontSendNotification);
    freqMaxEdit->setText(juce::String(params.freqMaxHz), juce::dontSendNotification);
    numPointsEdit->setText(juce::String(params.numPoints), juce::dontSendNotification);
    amplitudeEdit->setText(juce::String(params.amplitude), juce::dontSendNotification);
    repeatCountEdit->setText(juce::String(params.repeatCount), juce::dontSendNotification);
    logCheck->setToggleState(params.logarithmic, juce::dontSendNotification);
}

void StimulusForm::onFreqRangeEditChange()
{
    AnalysisController::Parameters params = analysisController->getParameters();
    params.freqMinHz = freqMinEdit->getText().getDoubleValue();
    params.freqMaxHz = freqMaxEdit->getText().getDoubleValue();
    analysisController->setParameters(params);
}

void StimulusForm::onNumPointsEditChange()
{
    AnalysisController::Parameters params = analysisController->getParameters();
    params.numPoints = numPointsEdit->getText().getIntValue();
    analysisController->setParameters(params);
}

void StimulusForm::onAmplitudeEditChange()
{
    AnalysisController::Parameters params = analysisController->getParameters();
    params.amplitude = amplitudeEdit->getText().getFloatValue();
    analysisController->setParameters(params);
}

void StimulusForm::onRepeatCountEditChange()
{
    AnalysisController::Parameters params = analysisController->getParameters();
    params.repeatCount = repeatCountEdit->getText().getIntValue();
    analysisController->setParameters(params);
}

void StimulusForm::onLogCheckClick()
{
    AnalysisController::Parameters params = analysisController->getParameters();
    params.logarithmic = logCheck->getToggleState();
    analysisController->setParameters(params);
}

void StimulusForm::onStartButtonClick()
{
    juce::Result r = analysisController->startProgressiveSession();
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
                 fixedSize="0" initialWidth="640" initialHeight="168">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="" id="232be4505a5dfa9e" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 96 16" edTextCol="ff000000" edBkgCol="0"
         labelText="Stimulus" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="1" italic="0" justification="33" typefaceStyle="Bold"/>
  <LABEL name="" id="fa33ea05dc713b35" memberName="freqRangeLabel" virtualName=""
         explicitFocusOrder="0" pos="16 32 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Frequency Range" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="a9c114409f5784b9" memberName="freqMinLabel" virtualName=""
         explicitFocusOrder="0" pos="144 32 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="min." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="13431d6471544dd5" memberName="freqMinEdit" virtualName=""
         explicitFocusOrder="0" pos="184 32 80 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="a141b8923c7bbb64" memberName="freqMaxLabel" virtualName=""
         explicitFocusOrder="0" pos="272 32 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="max." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="e55e43df6f4f9cb6" memberName="freqMaxEdit" virtualName=""
         explicitFocusOrder="0" pos="312 32 80 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="b0fcc681f65acb88" memberName="numPointsLabel" virtualName=""
         explicitFocusOrder="0" pos="16 64 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Measuring Points" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="53f962ea50e6e306" memberName="numPointsEdit" virtualName=""
         explicitFocusOrder="0" pos="184 64 80 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <TOGGLEBUTTON name="" id="5b694248e3aaab4b" memberName="logCheck" virtualName=""
                explicitFocusOrder="0" pos="272 64 120 24" buttonText="Logarithmic"
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <LABEL name="" id="886160caeb6b2a47" memberName="amplitudeLabel" virtualName=""
         explicitFocusOrder="0" pos="16 96 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Amplitude" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="b4bceb2cae3d1fa2" memberName="amplitudeEdit" virtualName=""
         explicitFocusOrder="0" pos="184 96 80 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="74c4cbbdc9c4ee60" memberName="repeatCountLabel" virtualName=""
         explicitFocusOrder="0" pos="16 128 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Repeat Count" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="8f8c3b8ebcd877e6" memberName="repeatCountEdit" virtualName=""
         explicitFocusOrder="0" pos="184 128 80 24" bkgCol="ffffffff"
         outlineCol="ffdddddd" edTextCol="ff000000" edBkgCol="0" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="34"/>
  <TEXTBUTTON name="" id="a2ea8d3d24ddd35a" memberName="startButton" virtualName=""
              explicitFocusOrder="0" pos="16Rr 128 96 24" buttonText="Start"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
} // namespace ST
} // namespace BAAPP
//[/EndFile]

