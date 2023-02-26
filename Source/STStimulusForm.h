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
#include "STAnalysisController.h"
namespace BAAPP {
namespace ST {
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
                      public juce::Label::Listener
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
    void onFreqRangeEditChange();
    void onNumPointsEditChange();
    void onAmplitudeEditChange();
    void onRepeatCountEditChange();
    void onLogCheckClick();
    void onStartButtonClick();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    juce::AudioDeviceManager* audioDeviceManager;
    AnalysisController::Ptr analysisController;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> freqRangeLabel;
    std::unique_ptr<juce::Label> freqMinLabel;
    std::unique_ptr<juce::Label> freqMinEdit;
    std::unique_ptr<juce::Label> freqMaxLabel;
    std::unique_ptr<juce::Label> freqMaxEdit;
    std::unique_ptr<juce::Label> numPointsLabel;
    std::unique_ptr<juce::Label> numPointsEdit;
    std::unique_ptr<juce::ToggleButton> logCheck;
    std::unique_ptr<juce::Label> amplitudeLabel;
    std::unique_ptr<juce::Label> amplitudeEdit;
    std::unique_ptr<juce::Label> repeatCountLabel;
    std::unique_ptr<juce::Label> repeatCountEdit;
    std::unique_ptr<juce::TextButton> startButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StimulusForm)
};

//[EndFile] You can add extra defines here...
} // namespace ST
} // namespace BAAPP
//[/EndFile]

