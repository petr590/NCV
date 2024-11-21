#ifndef NCV_ASSERT_H
#define NCV_ASSERT_H

#include "ncurses_wrap.h"
#include <cassert>
#include <cstdlib>

#ifndef NDEBUG

#define ASSERT(expr)\
		(static_cast<bool>(expr) ? void(0) :\
		__ncv_assert_fail(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr))

inline void __ncv_assert_fail(const char* file, unsigned int line,
		const char* function, const char* assertion) {

	if (!isendwin()) {
		ncurses_end();
	}

	fprintf(stderr, "%s:%u: %s: Assertion `%s' failed.\r\n", file, line, function, assertion);
	abort();
}



#define ASSERT_MESSAGE(expr, ...)\
		(static_cast<bool>(expr) ? void(0) :\
		false ? void(printf(__VA_ARGS__)) : /* Для проверки компилятором */ \
		__ncv_assert_fail_message(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__))

template<typename... Args>
inline void __ncv_assert_fail_message(const char* file, unsigned int line,
		const char* function, const char* assertion, Args... args) {

	if (!isendwin()) {
		ncurses_end();
	}

	fprintf(stderr, "%s:%u: %s: Assertion failed: ", file, line, function);
	fprintf(stderr, assertion, args...);
	fputs("\r\n", stderr);
	abort();
}

#else

#define ASSERT(expr) void(expr)
#define ASSERT_MESSAGE(expr, ...) __nothing(expr, __VA_ARGS__)

template<typename... Args>
inline void __nothing(Args...) {}

#endif /* NDEBUG */

#endif /* NCV_ASSERT_H */