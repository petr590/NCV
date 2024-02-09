#ifndef NCV_ASSERT
#define NCV_ASSERT

#include "ncurses_wrap.cpp"
#include <cassert>
#include <cstdlib>

#ifndef NDEBUG

#define ASSERT(expr)\
		(static_cast<bool>(expr) ? void(0) :\
		__ncv_assert_fail(__FILE__, __LINE__, __PRETTY_FUNCTION__, #expr))

void __ncv_assert_fail(const char* file, unsigned int line, const char* function, const char* assertion) {

	if (!isendwin()) {
		ncurses_end();
	}

	fprintf(stderr, "%s:%u: %s: Assertion `%s' failed.\r\n", file, line, function, assertion);
	abort();
}



#define ASSERT_MESSAGE(expr, ...)\
		(static_cast<bool>(expr) ? void(0) :\
		__ncv_assert_fail_message(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__))

template<typename... Args>
void __ncv_assert_fail_message(const char* file, unsigned int line, const char* function, const char* assertion, Args... args) {

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
#define ASSERT_MESSAGE(expr, ...) void(expr, __VA_ARGS__)

#endif /* NDEBUG */

#endif /* NCV_ASSERT */