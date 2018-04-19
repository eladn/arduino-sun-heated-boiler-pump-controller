#ifndef UI_SCREEN_MODE_INTERFACE_H_
#define UI_SCREEN_MODE_INTERFACE_H_

#include "UILcdInterface.h"

// TODO: implement!

class UIHandler;

class UIScreenModeInterfaceBase {
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;

	UIScreenModeInterfaceBase() {}
	virtual ~UIScreenModeInterfaceBase() {}
	virtual void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) = 0;
	UIScreenModeInterfaceBase* getNext() {
		// TODO: implement!
		return NULL;
	}
};

template <typename ButtonEventsArgType = void*>
class UIScreenModeInterface : public UIScreenModeInterfaceBase {
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;
	typedef EventsHandler<UIButtonEvent, ButtonEventsArgType> ButtonEventsHandlerType;
	
private:
	UIHandler* uiHandler;  // TODO: assign to when registering to it.
	struct BottonDetails {
		ButtonIdx buttonIdx;
		ButtonEventsHandlerType eventsHandler;
	} buttons[UI_MAX_BUTTONS];
	
public:
	UIScreenModeInterface()
		: UIScreenModeInterfaceBase(), uiHandler(NULL)
	{
		for (int i = 0; i < UI_MAX_BUTTONS; ++i) {
			this->buttons[i].buttonIdx = -1; // mark it as free to assign.
		}
	}
	
	virtual ~UIScreenModeInterface() {}
	
private:
	
	/* The following virtual methods can be overwritten by the inheritor class. */
	
	virtual void __init() {}
	virtual void __loop() {}
	
	virtual void __swichedIn() {}
	virtual void __swichedOut() {}
	
	
public:

	virtual void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) override {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) return;
		
		buttonDetails->eventsHandler.triggerEvents(event);
	}

	inline void init() {
		// TODO: implement!
		this->__init();
	}
	
	inline void loop() {
		// TODO: implement!
		this->__loop();
	}
	
	inline void swichedIn() {
		// TODO: implement!
		this->__swichedIn();
	}
	
	inline void swichedOut() {
		// TODO: implement!
		this->__swichedOut();
	}
	
	template <class CalleeClass>
	int registerHandler(ButtonIdx buttonIdx,
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy, 
						const ButtonEventsArgType& arg, 
						unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) {
			buttonDetails = this->findFreeBottonDetails();
			if (!buttonDetails) return -1;
			buttonDetails->buttonIdx = buttonIdx;
		}
		
		buttonDetails->eventsHandler = ButtonEventsHandlerType();
		
		return buttonDetails->eventsHandler.registerHandler<CalleeClass>(proxy, arg, eventsMask);
	}
	template <class CalleeClass>
	inline int onClick(ButtonIdx buttonIdx,
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy, 
						const ButtonEventsArgType& arg) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, arg, UIButtonEventType::Button_OnClick);
	}
	template <class CalleeClass>
	inline int onDoubleClick(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy,
						const ButtonEventsArgType& arg) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, arg, UIButtonEventType::Button_OnDoubleClick);
	}
	template <class CalleeClass>
	inline int onLongPress(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy,
						const ButtonEventsArgType& arg) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, arg, UIButtonEventType::Button_OnLongPress);
	}
	template <class CalleeClass>
	inline int onDown(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy,
						const ButtonEventsArgType& arg) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, arg, UIButtonEventType::Button_OnDown);
	}
	template <class CalleeClass>
	inline int onUp(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy,
						const ButtonEventsArgType& arg) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, arg, UIButtonEventType::Button_OnUp);
	}
	
	inline UILcdInterface* lcd(); /*{
		return this->uiHandler->lcd();
	}*/
	
private:

	inline BottonDetails* findBottonDetailsByIdx(ButtonIdx buttonIdx) {
		for (int i = 0; i < UI_MAX_BUTTONS; ++i) {
			if (this->buttons[i].buttonIdx == buttonIdx) return &this->buttons[i];
		}
		return NULL;
	}
	
	inline BottonDetails* findFreeBottonDetails() {
		for (int i = 0; i < UI_MAX_BUTTONS; ++i) {
			if (this->buttons[i].buttonIdx < 0) return &this->buttons[i];
		}
		return NULL;
	}
	
};

/*

// Example: This is a mode for testing.

class Mode1 : public UIScreenModeInterface<int> {
public:
	Mode1(UIHandler *uiHandler) : UIScreenModeInterface(uiHandler) {}
	virtual void __init() override {
		typedef ObjectMethodProxy<Mode1, void, UIButtonEvent, int> ProxyType;
		ProxyType proxy = ProxyType(this, &Mode1::m1);
		this->registerHandler<Mode1>(1, proxy, 1);
	}
	void m1(UIButtonEvent event, int t) {
		std::cout << "triggered: " << event.events << " " << t << std::endl;
	}
};

UIHandler uiHandler = UIHandler(0);
Mode1 mode1 = Mode1(&uiHandler);

void init() {
	uiHandler.setButton(1,1);
	mode1.init();
	mode1.buttonEventsOccurred(1,1);
}

*/

#endif /* UI_SCREEN_MODE_INTERFACE_H_ */
