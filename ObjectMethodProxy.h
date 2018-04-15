#ifndef OBJECT_METHOD_PROXY_H_
#define OBJECT_METHOD_PROXY_H_

#include <assert.h>

// http://en.cppreference.com/w/cpp/language/parameter_pack

template <typename ReturnType, typename ... ArgTypes>
class ObjectMethodProxyBase {
public:
	ObjectMethodProxyBase() {}
	virtual ~ObjectMethodProxyBase() {};
	virtual ReturnType call(ArgTypes ... args) = 0;
};

template <class CalleeClass, typename ReturnType, typename ... ArgTypes>
class ObjectMethodProxy
	: ObjectMethodProxyBase<ReturnType, ArgTypes ...> {
private:
	typedef ReturnType (CalleeClass::*ClassMethodPointer)(ArgTypes ... args);
	
	CalleeClass *objectPointer;
	ClassMethodPointer method;

public:
	ObjectMethodProxy()
	: objectPointer(NULL), method(NULL) {}
	
	ObjectMethodProxy(CalleeClass *objectPointer, ClassMethodPointer method)
	: objectPointer(objectPointer), method(method) {}
	
	virtual ReturnType call(ArgTypes ... args) override {
		assert(this->objectPointer != NULL && this->method != NULL);
		return (ReturnType)(((this->objectPointer)->*(this->method))(args...));
	}
};

#endif /* OBJECT_METHOD_PROXY_H_ */
