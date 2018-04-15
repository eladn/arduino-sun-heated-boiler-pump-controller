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

template <typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIButton {
public:
	typedef EventsHandler<ArgType, MaxEventHandlers> EventsHandlerType;
	
private:
	int pinNumber;
	EventsHandlerType eventsHandler;
	
public:
	UIButton(int pinNumber)
		: pinNumber(pinNumber),
		eventsHandler()
	{}
	
	template <class CalleeClass>
	inline int registerHandler(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg, 
				unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, eventsMask);
	}
	
	template <class CalleeClass>
	inline int onClick(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnClick);
	}
	
	template <class CalleeClass>
	inline int onDoubleClick(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnDoubleClick);
	}
	
	template <class CalleeClass>
	inline int onLongPress(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnLongPress);
	}
	
	template <class CalleeClass>
	inline int onDown(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnDown);
	}
	
	template <class CalleeClass>
	inline int onUp(
				const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnUp);
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
