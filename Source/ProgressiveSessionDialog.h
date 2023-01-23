//
//  ProgressiveSessionDialog.h
//  generalized behavior
//
//  created by yu2924 on 2023-02-09
//

#pragma once

#include "ProgressiveSessionBase.h"

juce::Component::SafePointer<juce::TopLevelWindow> progressiveSessionDialogShowAsync(ProgressiveSessionBase* ps, const juce::String& message);
