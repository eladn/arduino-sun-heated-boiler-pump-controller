#ifndef UI_HANDLER_H_
#define UI_HANDLER_H_

#define UI_MAX_BUTTONS 8

#include "UIButton.h"
#include "UIScreenModeInterface.h"


class UIHandler {
public:
	typedef int ButtonIdx;
	typedef UIButton<ButtonIdx, 1> ButtonType;
	typedef ObjectMethodProxy<UIHandler, void, unsigned int, ButtonIdx> ButtonProxyType;
	typedef UIButton<void*, 1> ModeButtonType;
	typedef ObjectMethodProxy<UIHandler, void, unsigned int, void*> ModeButtonProxyType;
	
private:
	ModeButtonType modeButton;
	
	// TODO: array of buttons!
	struct ButtonDetails {
		ButtonType button;
		ButtonIdx buttonIdx;
		
		ButtonDetails() : button(0), buttonIdx() {}
	} buttons[UI_MAX_BUTTONS];
	int nrButtons;
	
	UIScreenModeInterfaceBase *curScreenMode;
public:
	// TODO: implement!
	
	UIHandler(int modeButtonPin)
		: modeButton(modeButtonPin),
		nrButtons(0)
	{
		this->modeButton.onClick(ModeButtonProxyType(this, &UIHandler::switchToNextMode), NULL);
	}
	
	// Notice: Must NOT be called after init() has been called!
	void setButton(int buttonPin, ButtonIdx buttonIdx) {
		this->buttons[this->nrButtons].button = ButtonType(buttonPin);
		this->buttons[this->nrButtons].button.registerHandler(ButtonProxyType(this, &UIHandler::buttonEventsOccurred), buttonIdx);
		this->buttons[this->nrButtons].buttonIdx = buttonIdx;
		this->nrButtons++;
	}
	
	void switchToNextMode(unsigned int, void*) {
		assert(curScreenMode != NULL);
		
		// TODO: implement!
		
		//this->curScreenMode->swichedOut();
		//this->curScreenMode = this->curScreenMode->nextMode;
		//this->curScreenMode->swichedIn();
	}
	
	void buttonEventsOccurred(unsigned int events, ButtonIdx buttonIdx) {
		this->curScreenMode->buttonEventsOccurred(events, buttonIdx);
	}
	
	void init() {
		this->modeButton.init();
		
		for (int i = 0; i < this->nrButtons; ++i) {
			this->buttons[i].button.init();
		}
		
		// TODO: init modes!
	}
	
};

#endif /* UI_HANDLER_H_ */
