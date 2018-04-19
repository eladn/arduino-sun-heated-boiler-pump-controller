#ifndef EVENTS_HANDLER_H_
#define EVENTS_HANDLER_H_

#include <assert.h>
#include "static_assert.h"
#include "new_operator.h"
#include "ObjectMethodProxy.h"

#define DEFAULT_MAX_EVENT_HANDLERS 5


/* No argument */

template<typename EventType>
class EventHandler {
protected:
	class DummyCalleeClass {};
	typedef ObjectMethodProxy<DummyCalleeClass, void, EventType> DummyProxyType;
	
	char proxyBuffer[sizeof(DummyProxyType)];
	unsigned int eventsMask;

public:
	EventHandler() : eventsMask(0) {}
	
	// TODO: doc here! explain why I used this `DummyProxyType` and the `proxyBuffer`.
	
	template <class CalleeClass>
	void setMaskAndProxy(unsigned int eventsMask, const ObjectMethodProxy<CalleeClass, void, EventType>& proxy) {
		typedef ObjectMethodProxy<CalleeClass, void, EventType> ProxyType;
		
		if (!eventsMask) return;
		this->destructProxyIfExists();
		static_assert(sizeof(ProxyType) == sizeof(DummyProxyType), PROXYTYPE_HAVE_WRONG_SIZE);
		new(&this->proxyBuffer) ProxyType(proxy);
		this->eventsMask = eventsMask;
	}
	
	inline unsigned int getEventsMask() {
		return this->eventsMask;
	}
	
	inline ObjectMethodProxyBase<void, EventType>* getProxy() {
		return (ObjectMethodProxyBase<void, EventType> *)(&this->proxyBuffer);
	}
	
	~EventHandler() {
		this->destructProxyIfExists();
	}
	
	inline void triggerEvents(EventType& event) {
		if (event.events & this->eventsMask) {
			this->getProxy()->call(event);
		}
	}
	
	inline bool isSet() {
		return this->eventsMask;
	}
	
	protected:
	inline void destructProxyIfExists() {
		if (this->eventsMask) {
			this->getProxy()->~ObjectMethodProxyBase();
			this->eventsMask = 0;
		}
	}
};


template<typename EventType, typename ArgType>
class EventHandlerWithArg : public EventHandler<EventType> {
private:
	class DummyCalleeClass {};
	typedef ObjectMethodProxy<DummyCalleeClass, void, EventType, ArgType> DummyProxyType;
	
private:
	ArgType arg;

public:
	EventHandlerWithArg() : EventHandler<EventType>(), arg() {}
	
	template <class CalleeClass>
	void setMaskAndProxy(unsigned int eventsMask, const ObjectMethodProxy<CalleeClass, void, EventType, ArgType>& proxy) {
		if (!eventsMask) return;
		this->destructProxyIfExists();
		typedef ObjectMethodProxy<CalleeClass, void, EventType, ArgType> ProxyType;
		static_assert(sizeof(ProxyType) == sizeof(DummyProxyType), PROXYTYPE_HAVE_WRONG_SIZE);
		new(&this->proxyBuffer) ProxyType(proxy);
		this->eventsMask = eventsMask;
	}
	
	inline void setArg(const ArgType& arg) {
		this->arg = arg;
	}
	
	inline ArgType& getArg() {
		return this->arg;
	}
	
	inline ObjectMethodProxyBase<void, EventType, ArgType>* getProxy() {
		return (ObjectMethodProxyBase<void, EventType, ArgType> *)(&this->proxyBuffer);
	}
	
	~EventHandlerWithArg() {}
	
	inline void triggerEvents(EventType& event) {
		if (event.events & this->eventsMask) {
			this->getProxy()->call(event, this->arg);
		}
	}
};





/* EventsHandler type potentially contains multiple EventHandlers */

template<typename EventHandlerType, typename EventType, size_t MaxEventHandlers>
class EventsHandlerBase {
protected:
	EventHandlerType handlers[MaxEventHandlers];
	
public:
	EventsHandlerBase() {}
	
	~EventsHandlerBase() {
		/* I guess all handlers[] automatically destructed! */
	}
	
	void triggerEvents(EventType& event) {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			this->handlers[i].triggerEvents(event);
		}
	}
	
protected:
	template <class CalleeClass, typename ProxyType>
	EventHandlerType* __registerHandler(const ProxyType& proxy, 
						unsigned int eventsMask) {
		assert(eventsMask != 0);
		
		EventHandlerType* handler = this->findFreeHandler();
		if (!handler) return NULL;
		
		handler->setMaskAndProxy(eventsMask, proxy);
		
		return handler;
	}
	
	EventHandlerType* findFreeHandler() {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			if (!this->handlers[i].isSet()) return &this->handlers[i];
		}
		return NULL;
	}
};


template<typename EventType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandler : public EventsHandlerBase<EventHandler<EventType>, EventType, MaxEventHandlers> {
private:
	typedef EventHandler<EventType> EventHandlerType;
public:	
	EventsHandler() : EventsHandlerBase<EventHandler<EventType>, EventType, MaxEventHandlers>() {}
	~EventsHandler() {}
	
	template <class CalleeClass>
	int registerHandler(const ObjectMethodProxy<CalleeClass, void, EventType>& proxy, 
						unsigned int eventsMask) {
		typedef ObjectMethodProxy<CalleeClass, void, EventType> ProxyType;
		EventHandlerType* handler = this->template __registerHandler<CalleeClass, ProxyType>(proxy, eventsMask);
		if (!handler) return -1; // TODO: return meaningful error.
		return 0;
	}
};


template<typename EventType, typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandlerWithArg : public EventsHandlerBase<EventHandlerWithArg<EventType, ArgType>, EventType, MaxEventHandlers> {
private:
	typedef EventHandlerWithArg<EventType, ArgType> EventHandlerType;

public:
	EventsHandlerWithArg() : EventsHandlerBase<EventHandlerType, EventType, MaxEventHandlers>() {}
	~EventsHandlerWithArg() {}
	
	template <class CalleeClass>
	int registerHandler(const ObjectMethodProxy<CalleeClass, void, EventType, ArgType>& proxy, 
						ArgType arg, 
						unsigned int eventsMask) {
		typedef ObjectMethodProxy<CalleeClass, void, EventType, ArgType> ProxyType;
		EventHandlerType* handler = this->template __registerHandler<CalleeClass, ProxyType>(proxy, eventsMask);
		if (!handler) return -1; // TODO: return meaningful error.
		
		handler->setArg(arg);
		
		return 0;
	}
};

#endif /* EVENTS_HANDLER_H_ */
