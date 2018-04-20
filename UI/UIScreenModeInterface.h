#ifndef UI_SCREEN_MODE_INTERFACE_H_
#define UI_SCREEN_MODE_INTERFACE_H_

#include <assert.h>
#include "UILcdInterface.h"

// TODO: implement!

class UIHandler;

class UIScreenModeInterfaceBase {
protected:
	UIHandler* uiHandler;  // TODO: assign to when registering to it.
	UIScreenModeInterfaceBase* nextScreenMode;
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;

	UIScreenModeInterfaceBase() : uiHandler(NULL), nextScreenMode(this) {}
	virtual ~UIScreenModeInterfaceBase() {}
	virtual void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) = 0;
	UIScreenModeInterfaceBase* getNext() {
		return this->nextScreenMode;
	}
	void assignToUIHandler(UIHandler* uiHandler, UIScreenModeInterfaceBase* afterScreenMode) {
		assert(this->uiHandler == NULL && this->nextScreenMode != NULL);
		this->uiHandler = uiHandler;
		
		if (afterScreenMode != NULL) {
			this->nextScreenMode = afterScreenMode->nextScreenMode;
			afterScreenMode->nextScreenMode = this;
		}
		
	}
	
	inline void init() {
		assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
		this->__init();
	}
	
	inline void loop() {
		assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
		
		this->__loop_always();
		if (this->isTheCurrentChosenScreenMode()) {
			this->__loop();
		}
	}
	
	inline void swichedIn() {
		assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
		this->__swichedIn();
	}
	
	inline void swichedOut() {
		assert(this->uiHandler != NULL && this->nextScreenMode != NULL);
		this->__swichedOut();
	}
	
	inline UILcdInterface* lcd();
	
	inline bool isTheCurrentChosenScreenMode();
	
protected:

	/* The following virtual methods can be overwritten by the inheritor class. */
	
	virtual void __init() {}
	virtual void __loop() {}
	virtual void __loop_always() {}
	
	virtual void __swichedIn() {}
	virtual void __swichedOut() {}
};






template <typename ButtonEventsHandlerType, size_t MaxNumOfButtons = UI_MAX_BUTTONS>
class _UIScreenModeInterfaceBase : public UIScreenModeInterfaceBase {
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;
	
private:
	struct BottonDetails {
		ButtonIdx buttonIdx;
		ButtonEventsHandlerType eventsHandler;
	} buttons[MaxNumOfButtons];
	
public:
	_UIScreenModeInterfaceBase()
		: UIScreenModeInterfaceBase()
	{
		for (int i = 0; i < MaxNumOfButtons; ++i) {
			this->buttons[i].buttonIdx = -1; // mark it as free to assign.
		}
	}
	
	virtual ~_UIScreenModeInterfaceBase() {}
	
	virtual void buttonEventsOccurred(UIButtonEvent event, ButtonIdx buttonIdx) override {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) return;
		
		buttonDetails->eventsHandler.triggerEvents(event);
	}
	
protected:

	ButtonEventsHandlerType* getButtonEventsHandler(ButtonIdx buttonIdx) {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) {
			buttonDetails = this->findFreeBottonDetails();
			if (!buttonDetails) return -1;
			buttonDetails->buttonIdx = buttonIdx;
		}
		
		buttonDetails->eventsHandler = ButtonEventsHandlerType();
		
		return &buttonDetails->eventsHandler;
	}
	
	inline BottonDetails* findBottonDetailsByIdx(ButtonIdx buttonIdx) {
		for (int i = 0; i < MaxNumOfButtons; ++i) {
			if (this->buttons[i].buttonIdx == buttonIdx) return &this->buttons[i];
		}
		return NULL;
	}
	
	inline BottonDetails* findFreeBottonDetails() {
		for (int i = 0; i < MaxNumOfButtons; ++i) {
			if (this->buttons[i].buttonIdx < 0) return &this->buttons[i];
		}
		return NULL;
	}
};



template <size_t MaxNumOfButtons = UI_MAX_BUTTONS, size_t MaxNumOfButtonsEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIScreenModeInterface
	: public _UIScreenModeInterfaceBase< EventsHandler<UIButtonEvent, MaxNumOfButtonsEventHandlers>, MaxNumOfButtons >
{
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;
	typedef EventsHandler<UIButtonEvent, MaxNumOfButtonsEventHandlers> ButtonEventsHandlerType;
	
public:
	UIScreenModeInterface()
		: _UIScreenModeInterfaceBase<ButtonEventsHandlerType, MaxNumOfButtons>()
	{}
	
	virtual ~UIScreenModeInterface() {}

	template <class CalleeClass>
	int registerHandler(ButtonIdx buttonIdx,
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy, 
						unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		ButtonEventsHandlerType* eventsHandler = this->getButtonEventsHandler(buttonIdx);
		
		return eventsHandler->registerHandler<CalleeClass>(proxy, eventsMask);
	}
	template <class CalleeClass>
	inline int onClick(ButtonIdx buttonIdx,
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, UIButtonEventType::Button_OnClick);
	}
	template <class CalleeClass>
	inline int onDoubleClick(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, UIButtonEventType::Button_OnDoubleClick);
	}
	template <class CalleeClass>
	inline int onLongPress(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, UIButtonEventType::Button_OnLongPress);
	}
	template <class CalleeClass>
	inline int onDown(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, UIButtonEventType::Button_OnDown);
	}
	template <class CalleeClass>
	inline int onUp(ButtonIdx buttonIdx, 
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->registerHandler<CalleeClass>(buttonIdx, proxy, UIButtonEventType::Button_OnUp);
	}
};

template <typename ButtonEventsArgType, size_t MaxNumOfButtons = UI_MAX_BUTTONS, size_t MaxNumOfButtonsEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIScreenModeInterfaceWithButtonArg
	: public _UIScreenModeInterfaceBase< EventsHandlerWithArg<UIButtonEvent, ButtonEventsArgType, MaxNumOfButtonsEventHandlers>, MaxNumOfButtons >
{
public:
	//typedef UIHandler::ButtonIdx ButtonIdx; // TODO: fix this recursive include dependency issue!
	typedef int ButtonIdx;
	typedef EventsHandlerWithArg<UIButtonEvent, ButtonEventsArgType, MaxNumOfButtonsEventHandlers> ButtonEventsHandlerType;
	
public:
	UIScreenModeInterfaceWithButtonArg()
		: _UIScreenModeInterfaceBase<ButtonEventsHandlerType, MaxNumOfButtons>()
	{}
	
	virtual ~UIScreenModeInterfaceWithButtonArg() {}

	template <class CalleeClass>
	int registerHandler(ButtonIdx buttonIdx,
						const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ButtonEventsArgType>& proxy, 
						const ButtonEventsArgType& arg, 
						unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		ButtonEventsHandlerType* eventsHandler = this->getButtonEventsHandler(buttonIdx);
		
		return eventsHandler->registerHandler<CalleeClass>(proxy, arg, eventsMask);
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
};



/*

// Example: This is a mode for testing.

class Mode1 : public UIScreenModeInterfaceWithButtonArg<int> {
public:
	Mode1() : UIScreenModeInterfaceWithButtonArg() {}
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
Mode1 mode1 = Mode1();

void init() {
	uiHandler.setButton(1,1);
	mode1.init();
	mode1.buttonEventsOccurred(1,1);
}

*/

#endif /* UI_SCREEN_MODE_INTERFACE_H_ */
