//
//  ProgressiveSessionBase.h
//  generalized behavior
//
//  created by yu2924 on 2023-02-09
//

#pragma once

#include <JuceHeader.h>

class ProgressiveSessionBase : public juce::ReferenceCountedObject
{
public:
	struct Listener
	{
		virtual ~Listener() {}
		virtual void progressiveSessionBegin(ProgressiveSessionBase*) {}
		virtual void progressiveSessionEnd(ProgressiveSessionBase*, const juce::Result& r, bool aborted) { (void)r; (void)aborted; }
	};
	juce::ListenerList<Listener> progressiveSessionListenerList;
	virtual ~ProgressiveSessionBase() {}
	void addProgressiveSessionListener(Listener* p) { progressiveSessionListenerList.add(p); }
	void removeProgressiveSessionListener(Listener* p) { progressiveSessionListenerList.remove(p); }
	void callProgressiveSessionBegin() { progressiveSessionListenerList.call(&Listener::progressiveSessionBegin, this); }
	void callProgressiveSessionEnd(const juce::Result& r, bool aborted) { progressiveSessionListenerList.call(&Listener::progressiveSessionEnd, this, r, aborted); }
	virtual bool isProgressiveSessionRunning() const = 0;
	virtual double getProgressiveSessionProgress() const = 0;
	virtual void abortProgressiveSession() = 0;
};
