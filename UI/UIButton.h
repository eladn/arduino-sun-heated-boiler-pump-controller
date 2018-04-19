#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include "../utils/ObjectMethodProxy.h"
#include "../utils/EventsHandler.h"
#include "../utils/ShortTimeTimer.h"

/*
	Instructions for wiring the button:
	https://www.arduino.cc/en/Tutorial/Button
*/

// TODO: do we want to send the time pressed as well as the events? consider having event object instead of just `unsigned int events`.

struct UIButtonEvent {
	unsigned int events;
	unsigned long millisPressed;
};

enum UIButtonEventType {
	Button_OnClick =        (1<<0),
	Button_OnDoubleClick =  (1<<1),
	Button_OnLongPress =    (1<<2),
	Button_OnDown =         (1<<3),
	Button_OnUp =           (1<<4)
};

#define UI_BUTTON_EVENTS_FULL_MASK (UIButtonEventType::Button_OnClick || UIButtonEventType::Button_OnDoubleClick || UIButtonEventType::Button_OnLongPress || UIButtonEventType::Button_OnDown || UIButtonEventType::Button_OnUp)

enum UIButtonState { UP, DOWN };

template <typename EventsHandlerType>
class UIButtonBase {
protected:
	int pinNumber;
	EventsHandlerType eventsHandler;
	UIButtonState lastState;
	ShortTimeTimer timerSinceGotHigh;
	
public:
	UIButtonBase(int pinNumber)
		: pinNumber(pinNumber),
		eventsHandler(),
		lastState(UIButtonState::UP),
		timerSinceGotHigh()
	{}
	
	void init() {
		pinMode(this->pinNumber, INPUT);
	}
	
	void loop() {
		int buttonState = digitalRead(this->pinNumber);
		UIButtonEvent event;
		event.events = 0;
		event.millisPressed = 0;
		
		// TODO: find & set the relevant events occurred.
		if (buttonState == HIGH && this->lastState == UIButtonState::UP) {
			this->lastState == UIButtonState::DOWN;
			event.events |= UIButtonEventType::Button_OnDown;
		} else if (buttonState == HIGH && this->lastState == UIButtonState::DOWN) {
			
		} else if (buttonState == LOW && this->lastState == UIButtonState::UP) {
			
		} else if (buttonState == LOW && this->lastState == UIButtonState::DOWN) {
			
		} else {
			assert(0);
		}
		
		// TODO: implement!
		
		if (event.events) {
			this->eventsHandler.triggerEvents(event);
		}
		
	}
	
};


template <size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIButton : public UIButtonBase< EventsHandler<UIButtonEvent, MaxEventHandlers> > {
public:
	typedef EventsHandler<UIButtonEvent, MaxEventHandlers> EventsHandlerType;

	UIButton(int pinNumber)
		: UIButtonBase<EventsHandlerType>(pinNumber)
	{}
	
	template <class CalleeClass>
	inline int registerHandler(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy, 
				unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, eventsMask);
	}
	
	template <class CalleeClass>
	inline int onClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnClick);
	}
	
	template <class CalleeClass>
	inline int onDoubleClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnDoubleClick);
	}
	
	template <class CalleeClass>
	inline int onLongPress(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnLongPress);
	}
	
	template <class CalleeClass>
	inline int onDown(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnDown);
	}
	
	template <class CalleeClass>
	inline int onUp(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnUp);
	}
};


template <typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class UIButtonWithArg : public UIButtonBase< EventsHandlerWithArg<UIButtonEvent, ArgType, MaxEventHandlers> > {
public:
	typedef EventsHandlerWithArg<UIButtonEvent, ArgType, MaxEventHandlers> EventsHandlerType;

	UIButtonWithArg(int pinNumber)
		: UIButtonBase<EventsHandlerType>(pinNumber)
	{}
	
	template <class CalleeClass>
	inline int registerHandler(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg, 
				unsigned int eventsMask = UI_BUTTON_EVENTS_FULL_MASK) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, eventsMask);
	}
	
	template <class CalleeClass>
	inline int onClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnClick);
	}
	
	template <class CalleeClass>
	inline int onDoubleClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnDoubleClick);
	}
	
	template <class CalleeClass>
	inline int onLongPress(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnLongPress);
	}
	
	template <class CalleeClass>
	inline int onDown(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnDown);
	}
	
	template <class CalleeClass>
	inline int onUp(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnUp);
	}
};

#endif /* UI_BUTTON_H_ */
