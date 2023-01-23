//
//  ProgressiveSessionDialog.cpp
//  generalized behavior
//
//  created by yu2924 on 2023-02-09
//

#include "ProgressiveSessionDialog.h"

class ProgressiveSessionDialog
	: public juce::TopLevelWindow
	, public juce::Timer
	, public ProgressiveSessionBase::Listener
{
public:
	juce::ComponentDragger dragger;
	juce::ReferenceCountedObjectPtr<ProgressiveSessionBase> progressiveSession;
	juce::Label messageLabel;
	juce::ProgressBar progressBar;
	juce::TextButton cancelButton;
	double progressValue = 0;
	enum { Margin = 16, Spacing = 8, ButtonWidth = 96, ButtonHeight = 24, BarHeight = 20, LabelHeight = 20 };
	ProgressiveSessionDialog(ProgressiveSessionBase* ps, const juce::String& message)
		: TopLevelWindow("", true)
		, progressiveSession(ps)
		, progressBar(progressValue)
	{
		setUsingNativeTitleBar(false);
		setDropShadowEnabled(false);
		addAndMakeVisible(messageLabel);
		messageLabel.setText(message, juce::dontSendNotification);
		addAndMakeVisible(progressBar);
		addAndMakeVisible(cancelButton);
		cancelButton.setButtonText("Cancel");
		cancelButton.addShortcut(juce::KeyPress(juce::KeyPress::escapeKey));
		cancelButton.onClick = [this]()
		{
			progressiveSession->abortProgressiveSession();
			exitModalState(0);
		};
		setSize(400, Margin * 2 + LabelHeight + BarHeight + Spacing + ButtonHeight);
		startTimer(100);
		progressiveSession->addProgressiveSessionListener(this);
	}
	virtual ~ProgressiveSessionDialog()
	{
		progressiveSession->removeProgressiveSessionListener(this);
	}
	virtual int getDesktopWindowStyleFlags() const override
	{
		return 0;
	}
	virtual void resized() override
	{
		juce::Rectangle<int> rc = getLocalBounds().reduced(Margin);
		messageLabel.setBounds(rc.removeFromTop(LabelHeight));
		progressBar.setBounds(rc.removeFromTop(BarHeight));
		rc.removeFromTop(Spacing);
		cancelButton.setBounds(rc.removeFromTop(ButtonHeight).withSizeKeepingCentre(ButtonWidth, ButtonHeight));
	}
	virtual void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::white);
		juce::Rectangle<float> rc = getLocalBounds().toFloat().reduced(4);
		g.setColour(juce::Colours::lightgrey);
		g.drawRoundedRectangle(rc, 3, 2);
	}
	virtual void mouseDown(const juce::MouseEvent& me) override
	{
		dragger.startDraggingComponent(this, me);
	}
	virtual void mouseDrag(const juce::MouseEvent& me) override
	{
		dragger.dragComponent(this, me, nullptr);
	}
	virtual void timerCallback() override
	{
		progressValue = progressiveSession->getProgressiveSessionProgress();
		if(!progressiveSession->isProgressiveSessionRunning()) exitModalState(0);
	}
	virtual void progressiveSessionEnd(ProgressiveSessionBase*, const juce::Result&, bool) override
	{
		exitModalState(0);
	}
};

juce::Component::SafePointer<juce::TopLevelWindow> progressiveSessionDialogShowAsync(ProgressiveSessionBase* ps, const juce::String& message)
{
	ProgressiveSessionDialog* p = new ProgressiveSessionDialog(ps, message);
	p->centreAroundComponent(nullptr, p->getWidth(), p->getHeight());
	p->enterModalState(true, nullptr, true);
	return p;
}
