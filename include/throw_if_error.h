#ifndef NCV_THROW_IF_ERROR_H
#define NCV_THROW_IF_ERROR_H

#include "error_codes.h"
#include <stdexcept>

extern "C" {
	#include <libavformat/avformat.h>
}

/* Надстройка над функциями C, которые возвращают коды состояния. */
namespace ncv {
	#define throwIfError(func) __throwIfError<>(func, __FILE__, __LINE__, #func)

	#define throwIfErrorExcept(func, ...) __throwIfError<__VA_ARGS__>(func, __FILE__, __LINE__, #func)

	#define throwIfNull(func) __throwIfNull(func, __FILE__, __LINE__, #func)

	
	// Велосипеды, велосипеды... Всё же лучше, чем костыли с va_list
	template<typename T, T... values>
	struct any_of_equals {};

	template<typename T, T first, T... other>
	struct any_of_equals<T, first, other...> {
		constexpr inline bool equals(const T& value) {
			return value == first || any_of_equals<T, other...>().equals(value);
		}
	};

	template<typename T>
	struct any_of_equals<T> {
		constexpr inline bool equals(const T&) {
			return false;
		}
	};


	static int __throwError(const char* file, int line, const char* func, int result) {
		throw std::runtime_error(
			std::string("At ") + file + ':' + std::to_string(line) + ": " + func +
					"; return code = " + std::to_string(result)
		);
	}

	template<typename T>
	static T* __throwNullPtr(const char* file, int line, const char* func) {
		throw std::runtime_error(
			std::string("Null pointer at ") + file + ':' + std::to_string(line) + ": " + func
		);
	}


	template<int... excepts>
	inline int __throwIfError(int result, const char* file, int line, const char* func) {
		if (result == 0 || any_of_equals<int, excepts...>().equals(result))
			return result;
		
		return __throwError(file, line, func, result);
	}


	#define throwIfNegative(func) __throwIfNegative(func, __FILE__, __LINE__, #func)

	inline int __throwIfNegative(int result, const char* file, int line, const char* func) {
		if (result >= 0)
			return result;

		return __throwError(file, line, func, result);
	}


	template<typename T>
	inline T* __throwIfNull(T* result, const char* file, int line, const char* func) {
		if (result != nullptr)
			return result;

		return __throwNullPtr<T>(file, line, func);
	}
}

#endif /* NCV_THROW_IF_ERROR_H */
