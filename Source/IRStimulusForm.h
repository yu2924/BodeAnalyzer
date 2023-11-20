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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "IRAnalysisController.h"
#include "Plot2dPane.h"
namespace BAAPP {
namespace IR {
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class StimulusForm  : public juce::Component,
                      public juce::ChangeListener,
                      public ProgressiveSessionBase::Listener,
                      public juce::Label::Listener,
                      public juce::ComboBox::Listener
{
public:
    //==============================================================================
    StimulusForm (juce::AudioDeviceManager* adm, AnalysisController* ac);
    ~StimulusForm() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    // ChangeListener
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    // ProgressiveSessionBase::Listener
    virtual void progressiveSessionEnd(ProgressiveSessionBase*, const juce::Result&, bool) override;
    // internal
    void refrectProperties();
    void onExportButtonClick();
    void onMethodButtonClick();
    void onAmplitudeEditChnage();
    void onLengthComboChange();
    void onRepeatCountEditChnage();
    void onStartButtonClick();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    juce::AudioDeviceManager* audioDeviceManager;
    AnalysisController::Ptr analysisController;
    double currentSampleRate;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::TextButton> exportButton;
    std::unique_ptr<Plot2dPane> irPlotPane;
    std::unique_ptr<juce::ToggleButton> methodImpButton;
    std::unique_ptr<juce::ToggleButton> methodLinSSButton;
    std::unique_ptr<juce::ToggleButton> methodLogSSButton;
    std::unique_ptr<juce::ToggleButton> methodMLSButton;
    std::unique_ptr<juce::Label> amplitudeLabel;
    std::unique_ptr<juce::Label> amplitudeEdit;
    std::unique_ptr<juce::Label> lengthLabel;
    std::unique_ptr<juce::ComboBox> lengthCombo;
    std::unique_ptr<juce::Label> repeatCountLabel;
    std::unique_ptr<juce::Label> repeatCountEdit;
    std::unique_ptr<juce::TextButton> startButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StimulusForm)
};

//[EndFile] You can add extra defines here...
} // namespace IR
} // namespace BAAPP
//[/EndFile]

