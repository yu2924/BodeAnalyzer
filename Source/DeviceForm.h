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
#include "AnalysisController.h"
#include "LatencyProbeController.h"
namespace BAAPP {
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DeviceForm  : public juce::Component,
                    public juce::ChangeListener,
                    public ProgressiveSessionBase::Listener,
                    public juce::Label::Listener
{
public:
    //==============================================================================
    DeviceForm (juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc);
    ~DeviceForm() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    // ChangeListener
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    // ProgressiveSessionBase::Listener
    virtual void progressiveSessionEnd(ProgressiveSessionBase*, const juce::Result& r, bool aborted) override;
    // internal
    void reflectProperties();
    void onConfigureButtonClick();
    void onReopenButtonClick();
    void onRoundTripLatencyEditChange();
    void onProbeAmplitudeEditChange();
    void onProbeLengthEditChange();
    void onProbeDetectButtonClick();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    juce::AudioDeviceManager* audioDeviceManager;
    AnalysisController::Ptr analysisController;
    LatencyProbeController::Ptr latencyProbeController;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::TextButton> configureButton;
    std::unique_ptr<juce::TextButton> reopenButton;
    std::unique_ptr<juce::Label> roundTripLatencyLabel;
    std::unique_ptr<juce::Label> roundTripLatencyEdit;
    std::unique_ptr<juce::GroupComponent> probeGroup;
    std::unique_ptr<juce::Label> probeAmplitudeLabel;
    std::unique_ptr<juce::Label> probeAmplitudeEdit;
    std::unique_ptr<juce::Label> probeLengthLabel;
    std::unique_ptr<juce::Label> probeLengthEdit;
    std::unique_ptr<juce::TextButton> probeDetectButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceForm)
};

//[EndFile] You can add extra defines here...
} // namespace BAAPP
//[/EndFile]

