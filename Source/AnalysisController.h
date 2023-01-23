//
//  AnalysisController.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#pragma once

#include "ProgressiveSessionBase.h"
#include <vector>
#include <complex>

namespace BAAPP
{

	class AnalysisController : public ProgressiveSessionBase, public juce::ChangeBroadcaster
	{
	protected:
		AnalysisController() {}
	public:
		using Ptr = juce::ReferenceCountedObjectPtr<AnalysisController>;
		enum Method { MethodImpulse, MethodLinearSS, MethodLogSS };
		struct Parameters
		{
			Method method;
			int order; // length = 1 << order
			float amplitude;
			int repeatCount;
		};
		struct Response
		{
			std::vector<float> ir;
			std::vector<std::complex<double> > FR;
		};
		virtual double getCurrentSampleRate() const = 0;
		virtual const Parameters& getParameters() const = 0;
		virtual void setParameters(const Parameters& v) = 0;
		virtual int getRoundTripLatency() const = 0;
		virtual void setRoundTripLatency(int v) = 0;
		virtual const std::vector<float>& getStimulus() const = 0;
		virtual const std::vector<Response>& getResponseList() const = 0;
		virtual juce::Result startSession() = 0;
		static Ptr createInstance(juce::AudioDeviceManager* adm);
	};

} // namespace BAAPP
