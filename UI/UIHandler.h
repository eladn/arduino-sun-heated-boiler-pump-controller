#ifndef UI_HANDLER_H_
#define UI_HANDLER_H_

// TODO: make it a template size_t variable for UIHandler.
#define UI_MAX_BUTTONS 3

#include "UIButton.h"
#include "UIScreenModeInterface.h"
#include "UILcdInterface.h"

#define FOREACH_SCREENMODE(uiHandler, cur) \
	for (UIScreenModeInterfaceBase * cur = (uiHandler)->firstScreenMode; \
			cur != NULL; \
			cur = ((cur->getNext() != (uiHandler)->firstScreenMode) ? cur->getNext() : NULL))


class UIHandler {
public:
	typedef int ButtonIdx;
	typedef UIButtonWithArg<ButtonIdx, 1> ButtonType;
	typedef ObjectMethodProxy<UIHandler, void, UIButtonEvent, ButtonIdx> ButtonProxyType;
	typedef UIButton<1> ModeButtonType;
	typedef ObjectMethodProxy<UIHandler, void, UIButtonEvent> ModeButtonProxyType;
	
private:
	UILcdInterface *lcdInterface;
	ModeButtonType modeButton;
	
	struct ButtonDetails {
		ButtonType button;
		/*ButtonIdx buttonIdx;*/
		
		ButtonDetails() : button(0) /*, buttonIdx()*/ {}
	} buttons[UI_MAX_BUTTONS];
	int nrButtons;
	
	UIScreenModeInterfaceBase *firstScreenMode;
	UIScreenModeInterfaceBase *curScreenMode;
public:
	
	UIHandler(int modeButtonPin, UILcdInterface *lcdInterface)
		: lcdInterface(lcdInterface),
		modeButton(modeButtonPin),
		buttons(),
		nrButtons(0),
		firstScreenMode(NULL),
		curScreenMode(NULL)
	{
		this->modeButton.onClick(ModeButtonProxyType(this, &UIHandler::switchToNextMode));
	}
	
	void addScreenMode(UIScreenModeInterfaceBase *newScreenMode) {
		UIScreenModeInterfaceBase *lastScreenMode = this->getLastScreenMode();
		newScreenMode->assignToUIHandler(this, lastScreenMode);
		
		if (this->firstScreenMode == NULL) {
			this->firstScreenMode = newScreenMode;
		}
	}
	
	// Notice: Must NOT be called after init() has been called!
	void setButton(int buttonPin, ButtonIdx buttonIdx) {
		this->buttons[this->nrButtons].button = ButtonType(buttonPin);
		this->buttons[this->nrButtons].button.registerHandler(ButtonProxyType(this, &UIHandler::buttonEventsOccurred), buttonIdx);
		/*this->buttons[this->nrButtons].buttonIdx = buttonIdx;*/
		this->nrButtons++;
	}
	
	void switchToNextMode(UIButtonEvent) {
		assert(this->curScreenMode != NULL);
		this->curScreenMode->swichedOut();
		this->curScreenMode = this->curScreenMode->getNext();
		assert(this->curScreenMode != NULL);
		
		// TODO: consider writing the mode name before swiching into it.
		this->lcd()->clear();
		
		this->curScreenMode->swichedIn();
	}
	
	void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) {
		this->curScreenMode->buttonEventsOccurred(event, buttonIdx);
	}
	
	void init() {
		this->modeButton.init();
		
		for (int i = 0; i < this->nrButtons; ++i) {
			this->buttons[i].button.init();
		}
		
		FOREACH_SCREENMODE(this, screenMode) {
			screenMode->init();
		}
		
		this->curScreenMode = this->firstScreenMode;
		this->curScreenMode->swichedIn();
	}
	
	void loop() {
		this->modeButton.loop();
		
		for (int i = 0; i < this->nrButtons; ++i) {
			this->buttons[i].button.loop();
		}
		
		FOREACH_SCREENMODE(this, screenMode) {
			screenMode->loop();
		}
	}
	
	inline UILcdInterface* lcd() {
		return this->lcdInterface;
	}
	
	inline UIScreenModeInterfaceBase* getCurScreenMode() {
		return this->curScreenMode;
	}

private:
	UIScreenModeInterfaceBase* getLastScreenMode() {
		FOREACH_SCREENMODE(this, cur) {
			if (cur->getNext() == this->firstScreenMode) return cur;
		}
		return NULL;
	}
	
};

inline UILcdInterface* UIScreenModeInterfaceBase::lcd() {
	assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
	return this->uiHandler->lcd();
}

inline bool UIScreenModeInterfaceBase::isTheCurrentChosenScreenMode() {
	assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
	return (this->uiHandler->getCurScreenMode() == this);
}

#endif /* UI_HANDLER_H_ */
