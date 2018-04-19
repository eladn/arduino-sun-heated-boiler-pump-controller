#ifndef NEW_OPERATOR_H_
#define NEW_OPERATOR_H_

void* operator new(size_t size, void* ptr)
{
    return ptr;
}

#endif /* NEW_OPERATOR_H_ */
