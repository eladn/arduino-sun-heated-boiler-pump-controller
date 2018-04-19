#ifndef EVENTS_HANDLER_H_
#define EVENTS_HANDLER_H_

#include <assert.h>
#include "static_assert.h"
#include "new_operator.h"

#define DEFAULT_MAX_EVENT_HANDLERS 5


template<typename EventType, typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandler {
public:
	class DummyCalleeClass {};
	typedef ObjectMethodProxy<DummyCalleeClass, void, EventType, ArgType> DummyProxyType;
	
private:
	class EventHandler {
	private:
		char proxyBuffer[sizeof(DummyProxyType)];
		ArgType arg;
		unsigned int eventsMask;
	
	public:
		EventHandler() : eventsMask(0) {}
		
		inline ObjectMethodProxyBase<void, EventType, ArgType>* getProxy() {
			return (ObjectMethodProxyBase<void, EventType, ArgType> *)(&this->proxyBuffer);
		}
		
		// TODO: doc here! explain why I used this `DummyProxyType` and the `proxyBuffer`.
		
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
		
		inline unsigned int getEventsMask() {
			return this->eventsMask;
		}
		
		~EventHandler() {
			this->destructProxyIfExists();
		}
		
		inline void triggerEvents(EventType& event) {
			if (event.events & this->eventsMask) {
				this->getProxy()->call(event, this->arg);
			}
		}
		
		inline bool isSet() {
			return this->eventsMask;
		}
		
		private:
		inline void destructProxyIfExists() {
			if (this->eventsMask) {
				this->getProxy()->~ObjectMethodProxyBase();
				this->eventsMask = 0;
			}
		}
	} handlers[MaxEventHandlers];
	
public:
	EventsHandler() {}
	
	~EventsHandler() {
		/* I guess all handlers[] automatically destructed! */
	};
	
	template <class CalleeClass>
	int registerHandler(const ObjectMethodProxy<CalleeClass, void, EventType, ArgType>& proxy, 
						ArgType arg, 
						unsigned int eventsMask) {
		assert(eventsMask != 0);
		
		EventHandler* handler = this->findFreeHandler();
		if (!handler) return -1; // TODO: return meaningful error.
		
		handler->setMaskAndProxy(eventsMask, proxy);
		handler->setArg(arg);
		
		return 0;
	}
	
	void triggerEvents(EventType& event) {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			this->handlers[i].triggerEvents(event);
		}
	}
	
private:
	EventHandler* findFreeHandler() {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			if (!this->handlers[i].isSet()) return &this->handlers[i];
		}
		return NULL;
	}
};

#endif /* EVENTS_HANDLER_H_ */
