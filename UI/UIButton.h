#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include "../ObjectMethodProxy.h"
#include "../EventsHandler.h"

enum UIButtonEventType {
	Button_OnClick =        (1<<0),
	Button_OnDoubleClick =  (1<<1),
	Button_OnLongPress =    (1<<2),
	Button_OnDown =         (1<<3),
	Button_OnUp =           (1<<4)
};

#define UI_BUTTON_EVENTS_FULL_MASK (UIButtonEventType::Button_OnClick || UIButtonEventType::Button_OnDoubleClick || UIButtonEventType::Button_OnLongPress || UIButtonEventType::Button_OnDown || UIButtonEventType::Button_OnUp)

template <class CalleeClass, typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIButton {
public:
	typedef EventsHandler<CalleeClass, ArgType, MaxEventHandlers> EventsHandlerType;
	typedef typename EventsHandlerType::ProxyType ProxyType;
	
private:
	int pinNumber;
	EventsHandlerType eventsHandler;
	
public:
	UIButton(int pinNumber)
		: pinNumber(pinNumber),
		eventsHandler()
	{}
	
	inline int registerHandler(const ProxyType& proxy, const ArgType& arg, unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		return this->eventsHandler.registerHandler(proxy, arg, eventsMask);
	}
	inline int onClick(const ProxyType& proxy, const ArgType& arg) {
		return this->eventsHandler.registerHandler(proxy, arg, UIButtonEventType::Button_OnClick);
	}
	inline int onDoubleClick(const ProxyType& proxy, const ArgType& arg) {
		return this->eventsHandler.registerHandler(proxy, arg, UIButtonEventType::Button_OnDoubleClick);
	}
	inline int onLongPress(const ProxyType& proxy, const ArgType& arg) {
		return this->eventsHandler.registerHandler(proxy, arg, UIButtonEventType::Button_OnLongPress);
	}
	inline int onDown(const ProxyType& proxy, const ArgType& arg) {
		return this->eventsHandler.registerHandler(proxy, arg, UIButtonEventType::Button_OnDown);
	}
	inline int onUp(const ProxyType& proxy, const ArgType& arg) {
		return this->eventsHandler.registerHandler(proxy, arg, UIButtonEventType::Button_OnUp);
	}
	
	void init() {
		// TODO: implement!
	}
	
	void loop() {
		
		// TODO: implement!
		
		unsigned int events = 0;
		
		// TODO: find & set the relevant events occurred.
		
		if (events) {
			this->eventsHandler.triggerEvents(events);
		}
		
	}
	
	// TODO: implement!
	
};

#endif /* UI_BUTTON_H_ */
