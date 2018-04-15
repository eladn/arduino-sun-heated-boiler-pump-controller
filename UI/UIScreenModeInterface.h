#ifndef UI_SCREEN_MODE_INTERFACE_H_
#define UI_SCREEN_MODE_INTERFACE_H_


// TODO: implement!

class UIScreenModeInterfaceBase {
public:
	typedef UIHandler::ButtonIdx ButtonIdx;

	UIScreenModeInterfaceBase() {}
	virtual ~UIScreenModeInterfaceBase() {}
	virtual void buttonEventsOccurred(unsigned int events, ButtonIdx buttonIdx) = 0;
	// getNext();
};

template <typename ButtonEventsArgType>
class UIScreenModeInterface : public UIScreenModeInterfaceBase {
public:
	typedef UIHandler::ButtonIdx ButtonIdx;
	
	// TODO: doc here!
	class DummyCalleeClass {};
	typedef EventsHandler<DummyCalleeClass, ButtonEventsArgType> ButtonEventsHandlerDummyType;
	
private:
	UIHandler* uiHandler;
	struct BottonDetails {
		ButtonIdx buttonIdx;
		char eventsHandlerBuffer[sizeof(ButtonEventsHandlerDummyType)]; // TODO: doc here!
	} buttons[UI_MAX_BUTTONS];
	
public:
	UIScreenModeInterface(UIHandler* uiHandler)
		: UIScreenModeInterfaceBase(), uiHandler(uiHandler)
	{
		for (int i = 0; i < UI_MAX_BUTTONS; ++i) {
			this->buttons[i].buttonIdx = -1; // mark it as free to assign.
		}
	}
	
	virtual ~UIScreenModeInterface() {
		EventsHandlerBase<ButtonEventsArgType> *eventsHandler;
		
		// TODO: doc here!
		for (int i = 0; i < UI_MAX_BUTTONS; ++i) {
			if (this->buttons[i].buttonIdx < 0) continue;
			eventsHandler = (EventsHandlerBase<ButtonEventsArgType> *)(&this->buttons[i].eventsHandlerBuffer);
			eventsHandler->~EventsHandlerBase();
		}
	}
	
private:
	
	/* The following virtual methods can be overwritten by the inheritor class. */
	
	virtual void __init() {}
	virtual void __loop() {}
	
	virtual void __swichedIn() {}
	virtual void __swichedOut() {}
	
	
public:

	virtual void buttonEventsOccurred(unsigned int events, ButtonIdx buttonIdx) override {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) return;
		
		EventsHandlerBase<ButtonEventsArgType> *eventsHandler = (EventsHandlerBase<ButtonEventsArgType> *)(&buttonDetails->eventsHandlerBuffer);
		eventsHandler->triggerEvents(events);
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
	
	template <class InheritorClass>
	int registerHandler(ButtonIdx buttonIdx,
						const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy, 
						const ButtonEventsArgType& arg, 
						unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		BottonDetails* buttonDetails = this->findBottonDetailsByIdx(buttonIdx);
		if (!buttonDetails) {
			buttonDetails = this->findFreeBottonDetails();
			if (!buttonDetails) return -1;
			buttonDetails->buttonIdx = buttonIdx;
		}
		
		typedef EventsHandler<InheritorClass, ButtonEventsArgType> EventsHandlerType;
		EventsHandlerType *eventsHandler = new(&buttonDetails->eventsHandlerBuffer) EventsHandlerType();
		assert(eventsHandler);
		
		return eventsHandler->registerHandler(proxy, arg, eventsMask);
	}
	template <class InheritorClass>
	inline int onClick(ButtonIdx buttonIdx,
						const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy, 
						const ButtonEventsArgType& arg) {
		return this->registerHandler(buttonIdx, proxy, arg, UIButtonEventType::Button_OnClick);
	}
	template <class InheritorClass>
	inline int onDoubleClick(ButtonIdx buttonIdx, 
								const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy,
								const ButtonEventsArgType& arg) {
		return this->registerHandler(buttonIdx, proxy, arg, UIButtonEventType::Button_OnDoubleClick);
	}
	template <class InheritorClass>
	inline int onLongPress(ButtonIdx buttonIdx, 
							const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy,
							const ButtonEventsArgType& arg) {
		return this->registerHandler(buttonIdx, proxy, arg, UIButtonEventType::Button_OnLongPress);
	}
	template <class InheritorClass>
	inline int onDown(ButtonIdx buttonIdx, 
						const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy,
						const ButtonEventsArgType& arg) {
		return this->registerHandler(buttonIdx, proxy, arg, UIButtonEventType::Button_OnDown);
	}
	template <class InheritorClass>
	inline int onUp(ButtonIdx buttonIdx, 
					const typename EventsHandler<InheritorClass, ButtonEventsArgType>::ProxyType& proxy,
					const ButtonEventsArgType& arg) {
		return this->registerHandler(buttonIdx, proxy, arg, UIButtonEventType::Button_OnUp);
	}
	
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
		typedef typename EventsHandler<Mode1, int>::ProxyType ProxyType;
		ProxyType proxy = ProxyType(this, &Mode1::m1);
		this->registerHandler<Mode1>(1, proxy, 1);
	}
	void m1(unsigned int events, int t) {
		std::cout << "triggered: " << events << " " << t << std::endl;
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
