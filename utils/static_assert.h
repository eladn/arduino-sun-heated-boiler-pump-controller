#ifndef STATIC_ASSERT_H_
#define STATIC_ASSERT_H_

template <bool> struct CompileTimeError;
template <> struct CompileTimeError<true> {};
#define static_assert(cond,msg) { CompileTimeError<!!(cond)> ERROR_##msg; (void) ERROR_##msg; }

#endif /* STATIC_ASSERT_H_ */
