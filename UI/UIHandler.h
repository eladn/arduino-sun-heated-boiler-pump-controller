#ifndef UI_HANDLER_H_
#define UI_HANDLER_H_

#define UI_MAX_BUTTONS 8

#include "UIButton.h"
#include "UIScreenModeInterface.h"
#include "UILcdInterface.h"

class UIHandler {
public:
	typedef int ButtonIdx;
	typedef UIButton<ButtonIdx, 1> ButtonType;
	typedef ObjectMethodProxy<UIHandler, void, UIButtonEvent, ButtonIdx> ButtonProxyType;
	typedef UIButton<void*, 1> ModeButtonType;
	typedef ObjectMethodProxy<UIHandler, void, UIButtonEvent, void*> ModeButtonProxyType;
	
private:
	UILcdInterface *lcdInterface;
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
	
	UIHandler(int modeButtonPin, UILcdInterface *lcdInterface)
		: lcdInterface(lcdInterface),
		modeButton(modeButtonPin),
		buttons(),
		nrButtons(0)
	{
		this->modeButton.onClick(ModeButtonProxyType(this, &UIHandler::switchToNextMode), NULL);
	}
	
	void addScreenMode(UIScreenModeInterfaceBase *screenMode) {
		// TODO: implement!
	}
	
	// Notice: Must NOT be called after init() has been called!
	void setButton(int buttonPin, ButtonIdx buttonIdx) {
		this->buttons[this->nrButtons].button = ButtonType(buttonPin);
		this->buttons[this->nrButtons].button.registerHandler(ButtonProxyType(this, &UIHandler::buttonEventsOccurred), buttonIdx);
		this->buttons[this->nrButtons].buttonIdx = buttonIdx;
		this->nrButtons++;
	}
	
	void switchToNextMode(UIButtonEvent, void*) {
		assert(curScreenMode != NULL);
		
		// TODO: implement!
		
		//this->curScreenMode->swichedOut();
		//this->curScreenMode = this->curScreenMode->nextMode;
		//this->curScreenMode->swichedIn();
	}
	
	void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) {
		this->curScreenMode->buttonEventsOccurred(event, buttonIdx);
	}
	
	void init() {
		this->modeButton.init();
		
		for (int i = 0; i < this->nrButtons; ++i) {
			this->buttons[i].button.init();
		}
		
		// TODO: init modes!
	}
	
	void loop() {
		this->modeButton.loop();
		
		for (int i = 0; i < this->nrButtons; ++i) {
			this->buttons[i].button.loop();
		}
		
		// TODO: loop modes!
	}
	
	inline UILcdInterface* lcd() {
		return this->lcdInterface;
	}
	
};

#endif /* UI_HANDLER_H_ */
