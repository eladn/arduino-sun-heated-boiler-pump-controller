#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include "../utils/ObjectMethodProxy.h"
#include "../utils/EventsHandler.h"
#include "../utils/ShortTimeTimer.h"

/*
	Instructions for wiring the button:
	https://www.arduino.cc/en/Tutorial/Button
*/

#define IS_BUTTON_DOWN(pinNum) ((digitalRead(pinNum) == LOW) ? 1 : 0)

#define UIBUTTON_LONG_PRESS_MIN_MILLIS 2500
#define UIBUTTON_DOUBLE_CLICK_MAX_MILLIS_BETWEEN_CLICKS 500

struct UIButtonEvent {
	unsigned int events;
	unsigned long millisPressed;
};

// TODO: use these events in handlers-registrators.
enum UIButtonEventType {
	Button_OnClick =        (1<<0),
	Button_OnSingleClick =  (1<<1),
	Button_OnDoubleClick =  (1<<2),
	Button_OnLongPress =    (1<<3),
	Button_OnShortPress =   (1<<4),
	Button_OnDown =         (1<<5),
	Button_OnUp =           (1<<6)
};

#define UI_BUTTON_EVENTS_FULL_MASK (UIButtonEventType::Button_OnClick || UIButtonEventType::Button_OnDoubleClick || UIButtonEventType::Button_OnLongPress || UIButtonEventType::Button_OnDown || UIButtonEventType::Button_OnUp)

enum UIButtonState { UP, DOWN };

template <typename EventsHandlerType>
class UIButtonBase {
protected:
	int pinNumber;
	EventsHandlerType eventsHandler;
	UIButtonState lastState;
	ShortTimeTimer timerSinceChangedState;
	bool consecutivePresses;
	unsigned long lastPressDownMillis;
	
public:
	UIButtonBase(int pinNumber)
		: pinNumber(pinNumber),
		eventsHandler(),
		lastState(UIButtonState::UP),
		timerSinceChangedState(),
		consecutivePresses(false),
		lastPressDownMillis(0)
	{}
	
	void init() {
		pinMode(this->pinNumber, INPUT);  // TODO: use INPUT_PULLUP
	}
	
	// TODO: static logics check this functionality!
	void loop() {
		timerSinceChangedState.loop();
		
		int isButtonCurrentlyDown = IS_BUTTON_DOWN(this->pinNumber);
		UIButtonEvent event;
		event.events = 0;
		event.millisPressed = 0;
		
		// find & set the relevant events occurred.
		if (isButtonCurrentlyDown && this->lastState == UIButtonState::UP) {
			this->lastState == UIButtonState::DOWN;
			event.events |= UIButtonEventType::Button_OnDown;
			
			// mark whether this is a consecutive click (last clicked finished not long ago).
			if (timerSinceChangedState.isSet()
				&& !timerSinceChangedState.isPassedMillis(UIBUTTON_DOUBLE_CLICK_MAX_MILLIS_BETWEEN_CLICKS)) {
				this->consecutivePresses = true;
			} else {
				this->consecutivePresses = false;
			}
			
			timerSinceChangedState.reset();
			timerSinceChangedState.start();
		} else if (!isButtonCurrentlyDown && this->lastState == UIButtonState::DOWN) {
			this->lastState == UIButtonState::UP;
			event.events |= UIButtonEventType::Button_OnUp;
			event.events |= UIButtonEventType::Button_OnClick;
			
			this->lastPressDownMillis = timerSinceChangedState.millisPassed();
			event.millisPressed = this->lastPressDownMillis;
			
			if (timerSinceChangedState.isPassedMillis(UIBUTTON_LONG_PRESS_MIN_MILLIS)) {
				event.events |= UIButtonEventType::Button_OnLongPress;
			} else {
				event.events |= UIButtonEventType::Button_OnShortPress;
			}
			
			if (this->consecutivePresses) {
				this->consecutivePresses = false;
				event.events |= UIButtonEventType::Button_OnDoubleClick;
				// TODO: fix the following issue:
				// if there were k consecutive presses, `OnDoubleClick` event would fire k-1 times.
				// maybe we can use 3-state flag to remember the parity of the consecutive presses.
			}
			
			timerSinceChangedState.reset();
			timerSinceChangedState.start();
		} else if (this->lastState == UIButtonState::UP && timerSinceChangedState.isSet()
				&& timerSinceChangedState.isPassedMillis(UIBUTTON_DOUBLE_CLICK_MAX_MILLIS_BETWEEN_CLICKS)) {
			event.events |= UIButtonEventType::Button_OnSingleClick;
			event.millisPressed = this->lastPressDownMillis;
		}
		
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
	inline int onSingleClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnSingleClick);
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
	inline int onShortPress(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent>& proxy) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, UIButtonEventType::Button_OnShortPress);
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
	inline int onSingleeClick(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnSingleClick);
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
	inline int onShortPress(
				const ObjectMethodProxy<CalleeClass, void, UIButtonEvent, ArgType>& proxy, 
				const ArgType& arg) {
		return this->eventsHandler.registerHandler<CalleeClass>(proxy, arg, UIButtonEventType::Button_OnShortPress);
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
