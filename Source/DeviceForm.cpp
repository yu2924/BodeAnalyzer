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

#include "DeviceForm.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace BAAPP {
//[/MiscUserDefs]

//==============================================================================
DeviceForm::DeviceForm (juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc)
    : audioDeviceManager(adm), analysisController(ac), latencyProbeController(lpc)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    titleLabel.reset (new juce::Label (juce::String(),
                                       TRANS("Device Environment")));
    addAndMakeVisible (titleLabel.get());
    titleLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    titleLabel->setJustificationType (juce::Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    titleLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    titleLabel->setBounds (8, 8, 160, 16);

    configureButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (configureButton.get());
    configureButton->setButtonText (TRANS("Configure"));

    configureButton->setBounds (56, 32, 80, 24);

    reopenButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (reopenButton.get());
    reopenButton->setButtonText (TRANS("Reopen"));

    reopenButton->setBounds (144, 32, 80, 24);

    roundTripLatencyLabel.reset (new juce::Label (juce::String(),
                                                  TRANS("Round-Trip Latency")));
    addAndMakeVisible (roundTripLatencyLabel.get());
    roundTripLatencyLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    roundTripLatencyLabel->setJustificationType (juce::Justification::centredRight);
    roundTripLatencyLabel->setEditable (false, false, false);
    roundTripLatencyLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    roundTripLatencyLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    roundTripLatencyLabel->setBounds (8, 64, 136, 24);

    roundTripLatencyEdit.reset (new juce::Label (juce::String(),
                                                 juce::String()));
    addAndMakeVisible (roundTripLatencyEdit.get());
    roundTripLatencyEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    roundTripLatencyEdit->setJustificationType (juce::Justification::centredRight);
    roundTripLatencyEdit->setEditable (true, true, false);
    roundTripLatencyEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    roundTripLatencyEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    roundTripLatencyEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    roundTripLatencyEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    roundTripLatencyEdit->addListener (this);

    roundTripLatencyEdit->setBounds (144, 64, 80, 24);

    probeGroup.reset (new juce::GroupComponent (juce::String(),
                                                TRANS("Latency Probe")));
    addAndMakeVisible (probeGroup.get());

    probeGroup->setBounds (240, 8, 256, 88);

    probeAmplitudeLabel.reset (new juce::Label (juce::String(),
                                                TRANS("Amplitude")));
    addAndMakeVisible (probeAmplitudeLabel.get());
    probeAmplitudeLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    probeAmplitudeLabel->setJustificationType (juce::Justification::centredRight);
    probeAmplitudeLabel->setEditable (false, false, false);
    probeAmplitudeLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    probeAmplitudeLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    probeAmplitudeLabel->setBounds (240, 32, 80, 24);

    probeAmplitudeEdit.reset (new juce::Label (juce::String(),
                                               juce::String()));
    addAndMakeVisible (probeAmplitudeEdit.get());
    probeAmplitudeEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    probeAmplitudeEdit->setJustificationType (juce::Justification::centredRight);
    probeAmplitudeEdit->setEditable (true, true, false);
    probeAmplitudeEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    probeAmplitudeEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    probeAmplitudeEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    probeAmplitudeEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    probeAmplitudeEdit->addListener (this);

    probeAmplitudeEdit->setBounds (320, 32, 80, 24);

    probeLengthLabel.reset (new juce::Label (juce::String(),
                                             TRANS("Length")));
    addAndMakeVisible (probeLengthLabel.get());
    probeLengthLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    probeLengthLabel->setJustificationType (juce::Justification::centredRight);
    probeLengthLabel->setEditable (false, false, false);
    probeLengthLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    probeLengthLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    probeLengthLabel->setBounds (240, 64, 80, 24);

    probeLengthEdit.reset (new juce::Label (juce::String(),
                                            juce::String()));
    addAndMakeVisible (probeLengthEdit.get());
    probeLengthEdit->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    probeLengthEdit->setJustificationType (juce::Justification::centredRight);
    probeLengthEdit->setEditable (true, true, false);
    probeLengthEdit->setColour (juce::Label::backgroundColourId, juce::Colours::white);
    probeLengthEdit->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    probeLengthEdit->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    probeLengthEdit->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    probeLengthEdit->addListener (this);

    probeLengthEdit->setBounds (320, 64, 80, 24);

    probeDetectButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (probeDetectButton.get());
    probeDetectButton->setButtonText (TRANS("Detect"));

    probeDetectButton->setBounds (408, 32, 80, 24);


    //[UserPreSize]
    configureButton->onClick = [this]() { onConfigureButtonClick(); };
    reopenButton->onClick = [this]() {onReopenButtonClick(); };
    roundTripLatencyEdit->onTextChange = [this]() { onRoundTripLatencyEditChange(); };
    probeAmplitudeEdit->onTextChange = [this]() { onProbeAmplitudeEditChange(); };
    probeLengthEdit->onTextChange = [this]() { onProbeLengthEditChange(); };
    probeDetectButton->onClick = [this]() { onProbeDetectButtonClick(); };
    reflectProperties();
    //[/UserPreSize]

    setSize (640, 104);


    //[Constructor] You can add your own custom stuff here..
    audioDeviceManager->addChangeListener(this);
    analysisController->addChangeListener(this);
    latencyProbeController->addChangeListener(this);
    latencyProbeController->addProgressiveSessionListener(this);
    //[/Constructor]
}

DeviceForm::~DeviceForm()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    latencyProbeController->removeProgressiveSessionListener(this);
    latencyProbeController->removeChangeListener(this);
    analysisController->removeChangeListener(this);
    audioDeviceManager->removeChangeListener(this);
    //[/Destructor_pre]

    titleLabel = nullptr;
    configureButton = nullptr;
    reopenButton = nullptr;
    roundTripLatencyLabel = nullptr;
    roundTripLatencyEdit = nullptr;
    probeGroup = nullptr;
    probeAmplitudeLabel = nullptr;
    probeAmplitudeEdit = nullptr;
    probeLengthLabel = nullptr;
    probeLengthEdit = nullptr;
    probeDetectButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DeviceForm::paint (juce::Graphics& g)
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

void DeviceForm::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DeviceForm::labelTextChanged (juce::Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == roundTripLatencyEdit.get())
    {
        //[UserLabelCode_roundTripLatencyEdit] -- add your label text handling code here..
        //[/UserLabelCode_roundTripLatencyEdit]
    }
    else if (labelThatHasChanged == probeAmplitudeEdit.get())
    {
        //[UserLabelCode_probeAmplitudeEdit] -- add your label text handling code here..
        //[/UserLabelCode_probeAmplitudeEdit]
    }
    else if (labelThatHasChanged == probeLengthEdit.get())
    {
        //[UserLabelCode_probeLengthEdit] -- add your label text handling code here..
        //[/UserLabelCode_probeLengthEdit]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


// --------------------------------------------------------------------------------
// ChangeListener
void DeviceForm::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == audioDeviceManager)
    {
        reflectProperties();
    }
    else if(source == analysisController.get())
    {
        reflectProperties();
    }
    else if(source == latencyProbeController.get())
    {
        reflectProperties();
    }
}

// --------------------------------------------------------------------------------
// ProgressiveSessionBase::Listener

void DeviceForm::progressiveSessionEnd(ProgressiveSessionBase* ps, const juce::Result& r, bool aborted)
{
    if(ps == latencyProbeController.get())
    {
        if(r.failed() && !aborted)
        {
            juce::MessageManager::callAsync([this, r]() { juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", r.getErrorMessage()); });
        }
        else if(r.wasOk())
        {
            int latency = latencyProbeController->getResultLatency();
            analysisController->setRoundTripLatency(latency);
        }
    }
}

// --------------------------------------------------------------------------------
// internal

void DeviceForm::reflectProperties()
{
    juce::AudioDeviceManager::AudioDeviceSetup ads = audioDeviceManager->getAudioDeviceSetup();
    juce::String inf;
    inf << audioDeviceManager->getCurrentAudioDeviceType() << "\n"
        << "output: " << ads.outputDeviceName.quoted() << " " << ads.outputChannels.countNumberOfSetBits() << "ch\n"
        << "input : " << ads.inputDeviceName.quoted() << " " << ads.inputChannels.countNumberOfSetBits() << "ch\n"
        << "fs=" << ads.sampleRate;
    configureButton->setTooltip(inf);
    reopenButton->setTooltip(inf);
    roundTripLatencyEdit->setText(juce::String(analysisController->getRoundTripLatency()), juce::dontSendNotification);
    probeAmplitudeEdit->setText(juce::String(latencyProbeController->getProbeAmplitude()), juce::dontSendNotification);
    probeLengthEdit->setText(juce::String(latencyProbeController->getProbeLength()), juce::dontSendNotification);
}

void DeviceForm::onConfigureButtonClick()
{
    juce::AudioDeviceSelectorComponent* pane = new juce::AudioDeviceSelectorComponent(*audioDeviceManager, 1, 8, 1, 8, false, false, false, false);
    pane->setSize(640, 640);
    juce::DialogWindow::LaunchOptions opt;
    opt.dialogTitle = "Device Settings";
    opt.content.setOwned(pane);
    opt.launchAsync();
}

void DeviceForm::onReopenButtonClick()
{
    audioDeviceManager->closeAudioDevice();
    audioDeviceManager->restartLastAudioDevice();
}

void DeviceForm::onRoundTripLatencyEditChange()
{
    analysisController->setRoundTripLatency(roundTripLatencyEdit->getText().getIntValue());
}

void DeviceForm::onProbeAmplitudeEditChange()
{
    latencyProbeController->setProbeAmplitude(probeAmplitudeEdit->getText().getFloatValue());
}

void DeviceForm::onProbeLengthEditChange()
{
    latencyProbeController->setProbeLength(probeLengthEdit->getText().getIntValue());
}

void DeviceForm::onProbeDetectButtonClick()
{
    juce::Result r = latencyProbeController->startSession();
    if(r.wasOk()) progressiveSessionDialogShowAsync(latencyProbeController.get(), "now measuring...");
    else juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", r.getErrorMessage());
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DeviceForm" componentName=""
                 parentClasses="public juce::Component, public juce::ChangeListener, public ProgressiveSessionBase::Listener"
                 constructorParams="juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc"
                 variableInitialisers="audioDeviceManager(adm), analysisController(ac), latencyProbeController(lpc)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="640" initialHeight="104">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="" id="682661271ee3a25" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 160 16" edTextCol="ff000000"
         edBkgCol="0" labelText="Device Environment" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <TEXTBUTTON name="" id="2ca37b697dfce8de" memberName="configureButton" virtualName=""
              explicitFocusOrder="0" pos="56 32 80 24" buttonText="Configure"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="" id="fcf00c898fddffec" memberName="reopenButton" virtualName=""
              explicitFocusOrder="0" pos="144 32 80 24" buttonText="Reopen"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <LABEL name="" id="8041f23a57dca877" memberName="roundTripLatencyLabel"
         virtualName="" explicitFocusOrder="0" pos="8 64 136 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Round-Trip Latency" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="239f0ecadf7eae89" memberName="roundTripLatencyEdit"
         virtualName="" explicitFocusOrder="0" pos="144 64 80 24" bkgCol="ffffffff"
         outlineCol="ffdddddd" edTextCol="ff000000" edBkgCol="0" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="34"/>
  <GROUPCOMPONENT name="" id="f37b1a51f9e07b3f" memberName="probeGroup" virtualName=""
                  explicitFocusOrder="0" pos="240 8 256 88" title="Latency Probe"/>
  <LABEL name="" id="6241b7864c61818" memberName="probeAmplitudeLabel"
         virtualName="" explicitFocusOrder="0" pos="240 32 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Amplitude" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="43c6b7246707cf3" memberName="probeAmplitudeEdit"
         virtualName="" explicitFocusOrder="0" pos="320 32 80 24" bkgCol="ffffffff"
         outlineCol="ffdddddd" edTextCol="ff000000" edBkgCol="0" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="34"/>
  <LABEL name="" id="2ca07b5d44048209" memberName="probeLengthLabel" virtualName=""
         explicitFocusOrder="0" pos="240 64 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Length" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="604061d1c2955615" memberName="probeLengthEdit" virtualName=""
         explicitFocusOrder="0" pos="320 64 80 24" bkgCol="ffffffff" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <TEXTBUTTON name="" id="d9f60cd1579ea5b2" memberName="probeDetectButton"
              virtualName="" explicitFocusOrder="0" pos="408 32 80 24" buttonText="Detect"
              connectedEdges="0" needsCallback="0" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
} // namespace BAAPP
//[/EndFile]

