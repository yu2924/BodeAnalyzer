//
//  IRResponsePane.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-06
//

#pragma once

#include <JuceHeader.h>
#include "IRAnalysisController.h"

namespace BAAPP
{
	namespace IR
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

	} // namespace IR
} // namespace BAAPP
