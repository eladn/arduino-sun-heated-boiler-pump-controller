#ifndef EVENTS_HANDLER_H_
#define EVENTS_HANDLER_H_

#define DEFAULT_MAX_EVENT_HANDLERS 5

template <typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandlerBase {
public:
	EventsHandlerBase() {};
	virtual ~EventsHandlerBase() {};
	virtual void triggerEvents(unsigned int events) = 0;
};

template<class CalleeClass, typename ArgType, size_t MaxEventHandlers = DEFAULT_MAX_EVENT_HANDLERS>
class EventsHandler : public EventsHandlerBase<ArgType, MaxEventHandlers> {
public:
	typedef ObjectMethodProxy<CalleeClass, void, unsigned int, ArgType> ProxyType;
private:
	struct EventHandler {
		ProxyType proxy;
		ArgType arg;
		unsigned int eventsMask;
	} handlers[MaxEventHandlers];
	
public:
	EventsHandler()
		: EventsHandlerBase<ArgType, MaxEventHandlers>()
	{
		// zero the event masks for all handlers so they won't be called.
		for (int i = 0; i < MaxEventHandlers; ++i) {
			this->handlers[i].eventsMask = 0;
		}
	}
	virtual ~EventsHandler() {};
	
	int registerHandler(const ProxyType& proxy, 
						ArgType arg, 
						unsigned int eventsMask) {
		assert(eventsMask != 0);
		
		EventHandler* handler = this->findFreeHandler();
		if (!handler) return -1; // TODO: return meaningful error.
		
		handler->proxy = proxy;
		handler->arg = arg;
		handler->eventsMask = eventsMask; // != 0
		
		return 0;
	}
	
	virtual void triggerEvents(unsigned int events) override {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			if (events & this->handlers[i].eventsMask) {
				this->handlers[i].proxy.call(events, this->handlers[i].arg);
			}
		}
	}
	
private:
	EventHandler* findFreeHandler() {
		for (int i = 0; i < MaxEventHandlers; ++i) {
			if (this->handlers[i].eventsMask == 0) return &this->handlers[i];
		}
		return NULL;
	}
};

#endif /* EVENTS_HANDLER_H_ */
