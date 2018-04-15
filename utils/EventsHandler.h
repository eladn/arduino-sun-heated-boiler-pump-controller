#ifndef EVENTS_HANDLER_H_
#define EVENTS_HANDLER_H_

#define DEFAULT_MAX_EVENT_HANDLERS 5


template<typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandler {
public:
	class DummyCalleeClass {};
	typedef ObjectMethodProxy<DummyCalleeClass, void, unsigned int, ArgType> DummyProxyType;
	
private:
	class EventHandler {
	private:
		char proxyBuffer[sizeof(DummyProxyType)];
		ArgType arg;
		unsigned int eventsMask;
	
	public:
		EventHandler() : eventsMask(0) {}
		
		ObjectMethodProxyBase<void, unsigned int, ArgType>* getProxy() {
			return (ObjectMethodProxyBase<void, unsigned int, ArgType> *)(&this->proxyBuffer);
		}
		
		// TODO: doc here! explain why I used this `DummyProxyType` and the `proxyBuffer`.
		
		template <class CalleeClass>
		void setMaskAndProxy(unsigned int eventsMask, const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy) {
			this->destructProxyIfExists();
			typedef ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType> ProxyType;
			assert(sizeof(ProxyType) == sizeof(DummyProxyType)); // TODO: static_assert() it!
			new(&this->proxyBuffer) ProxyType(proxy);
			this->eventsMask = eventsMask;
		}
		
		void setArg(const ArgType& arg) {
			this->arg = arg;
		}
		
		ArgType& getArg() {
			return this->arg;
		}
		
		unsigned int getEventsMask() {
			return this->eventsMask;
		}
		
		~EventHandler() {
			this->destructProxyIfExists();
		}
		
		void triggerEvents(unsigned int events) {
			if (events & this->eventsMask) {
				this->getProxy()->call(events, this->arg);
			}
		}
		
		bool isSet() {
			return this->eventsMask;
		}
		
		private:
		void destructProxyIfExists() {
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
	int registerHandler(const ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType>& proxy, 
						ArgType arg, 
						unsigned int eventsMask) {
		assert(eventsMask != 0);
		
		EventHandler* handler = this->findFreeHandler();
		if (!handler) return -1; // TODO: return meaningful error.
		
		handler->setMaskAndProxy(eventsMask, proxy);
		handler->setArg(arg);
		
		return 0;
	}
	
	void triggerEvents(unsigned int events) {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			this->handlers[i].triggerEvents(events);
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
