//
//  STResponsePane.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-22
//

#pragma once

#include <JuceHeader.h>
#include "STAnalysisController.h"

namespace BAAPP
{
	namespace ST
	{

		class ResponsePane : public juce::Component
		{
		protected:
			class Impl;
			Impl* impl;
		public:
			ResponsePane(AnalysisController* ac);
			virtual ~ResponsePane();
			virtual void resized() override;
			virtual void paint(juce::Graphics& g) override;
		};

	} // namespace ST
} // namespace BAAPP
