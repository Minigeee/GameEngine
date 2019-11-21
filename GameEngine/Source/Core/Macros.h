#ifndef MACROS_H
#define MACROS_H

#include <string>

///////////////////////////////////////////////////////////////////////////////

#define __CONCAT(a, b) a##b

#define AUG(...) _, __VA_ARGS__
#define EXPAND(x) x
#define CONCAT(a, b) __CONCAT(a, b)
#define STR(x) #x

#ifdef WIN32
#define FILE_SEPERATOR '\\'
#else
#define FILE_SEPERATOR '/'
#endif
#define FILE_NAME (strrchr(__FILE__, FILE_SEPERATOR) ? strrchr(__FILE__, FILE_SEPERATOR) + 1 : __FILE__)

///////////////////////////////////////////////////////////////////////////////

/* Get number of arguments */

#ifdef _MSC_VER

#define NARGS_COUNT( \
	_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
	_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
	_21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
	_31, _32, _33, _34, _35, _36, _37, _38, _39, _40, x, ...) x
#define NARGS_EXPAND_ARGS(...) EXPAND(NARGS_COUNT( \
	__VA_ARGS__, \
	39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
	9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define NARGS(...) NARGS_EXPAND_ARGS(AUG(__VA_ARGS__))
#define FIRST_ARG(x, ...) x
#define NFIRST_ARGS(x, ...) EXPAND(__VA_ARGS__)

#endif

///////////////////////////////////////////////////////////////////////////////

#define RECURSIVE_0(func, x, ...)
#define RECURSIVE_1(func, x, ...) func(x)
#define RECURSIVE_2(func, x, ...) func(x) EXPAND(RECURSIVE_1(func, __VA_ARGS__))
#define RECURSIVE_3(func, x, ...) func(x) EXPAND(RECURSIVE_2(func, __VA_ARGS__))
#define RECURSIVE_4(func, x, ...) func(x) EXPAND(RECURSIVE_3(func, __VA_ARGS__))
#define RECURSIVE_5(func, x, ...) func(x) EXPAND(RECURSIVE_4(func, __VA_ARGS__))
#define RECURSIVE_6(func, x, ...) func(x) EXPAND(RECURSIVE_5(func, __VA_ARGS__))
#define RECURSIVE_7(func, x, ...) func(x) EXPAND(RECURSIVE_6(func, __VA_ARGS__))
#define RECURSIVE_8(func, x, ...) func(x) EXPAND(RECURSIVE_7(func, __VA_ARGS__))
#define RECURSIVE_9(func, x, ...) func(x) EXPAND(RECURSIVE_8(func, __VA_ARGS__))
#define RECURSIVE_10(func, x, ...) func(x) EXPAND(RECURSIVE_9(func, __VA_ARGS__))
#define RECURSIVE_11(func, x, ...) func(x) EXPAND(RECURSIVE_10(func, __VA_ARGS__))
#define RECURSIVE_12(func, x, ...) func(x) EXPAND(RECURSIVE_11(func, __VA_ARGS__))
#define RECURSIVE_13(func, x, ...) func(x) EXPAND(RECURSIVE_12(func, __VA_ARGS__))
#define RECURSIVE_14(func, x, ...) func(x) EXPAND(RECURSIVE_13(func, __VA_ARGS__))
#define RECURSIVE_15(func, x, ...) func(x) EXPAND(RECURSIVE_14(func, __VA_ARGS__))
#define RECURSIVE_16(func, x, ...) func(x) EXPAND(RECURSIVE_15(func, __VA_ARGS__))
#define RECURSIVE_17(func, x, ...) func(x) EXPAND(RECURSIVE_16(func, __VA_ARGS__))
#define RECURSIVE_18(func, x, ...) func(x) EXPAND(RECURSIVE_17(func, __VA_ARGS__))
#define RECURSIVE_19(func, x, ...) func(x) EXPAND(RECURSIVE_18(func, __VA_ARGS__))
#define RECURSIVE_20(func, x, ...) func(x) EXPAND(RECURSIVE_19(func, __VA_ARGS__))
#define RECURSIVE_21(func, x, ...) func(x) EXPAND(RECURSIVE_20(func, __VA_ARGS__))
#define RECURSIVE_22(func, x, ...) func(x) EXPAND(RECURSIVE_21(func, __VA_ARGS__))
#define RECURSIVE_23(func, x, ...) func(x) EXPAND(RECURSIVE_22(func, __VA_ARGS__))
#define RECURSIVE_24(func, x, ...) func(x) EXPAND(RECURSIVE_23(func, __VA_ARGS__))
#define RECURSIVE_25(func, x, ...) func(x) EXPAND(RECURSIVE_24(func, __VA_ARGS__))
#define RECURSIVE_26(func, x, ...) func(x) EXPAND(RECURSIVE_25(func, __VA_ARGS__))
#define RECURSIVE_27(func, x, ...) func(x) EXPAND(RECURSIVE_26(func, __VA_ARGS__))
#define RECURSIVE_28(func, x, ...) func(x) EXPAND(RECURSIVE_27(func, __VA_ARGS__))
#define RECURSIVE_29(func, x, ...) func(x) EXPAND(RECURSIVE_28(func, __VA_ARGS__))
#define RECURSIVE_30(func, x, ...) func(x) EXPAND(RECURSIVE_29(func, __VA_ARGS__))
#define RECURSIVE_31(func, x, ...) func(x) EXPAND(RECURSIVE_30(func, __VA_ARGS__))
#define RECURSIVE_32(func, x, ...) func(x) EXPAND(RECURSIVE_31(func, __VA_ARGS__))

#define LOOP(expr, ...) EXPAND(CONCAT(RECURSIVE_, NARGS(__VA_ARGS__))(expr, __VA_ARGS__))
#define LOOP_XTIMES(N, expr, ...) EXPAND(CONCAT(RECURSIVE_, N)(expr, __VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////

#define COMMA_LIST(func, comma_func, ...) func(FIRST_ARG(__VA_ARGS__))LOOP(comma_func, NFIRST_ARGS(__VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////

#endif