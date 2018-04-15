#ifndef OBJECT_METHOD_PROXY_H_
#define OBJECT_METHOD_PROXY_H_

#include <assert.h>

// http://en.cppreference.com/w/cpp/language/parameter_pack

template <class T, typename ReturnType, typename ... ArgTypes>
class ObjectMethodProxy {
private:
	typedef ReturnType (T::*ClassMethodPointer)(ArgTypes ... args);
	
	T *objectPointer;
	ClassMethodPointer method;

public:
	ObjectMethodProxy()
	: objectPointer(NULL), method(NULL) {}
	
	ObjectMethodProxy(T *objectPointer, ClassMethodPointer method)
	: objectPointer(objectPointer), method(method) {}
	
	inline ReturnType call(ArgTypes ... args) {
		assert(this->objectPointer != NULL && this->method != NULL);
		return (ReturnType)(((this->objectPointer)->*(this->method))(args...));
	}
};

#endif /* OBJECT_METHOD_PROXY_H_ */
