#ifndef NCV_PAIR_UTILS_H
#define NCV_PAIR_UTILS_H

#include <tuple>

namespace ncv {
	template<typename T>
	std::pair<T, T> sorted_pair(const T& first, const T& second) {
		return first < second ?
				std::make_pair(first, second) :
				std::make_pair(second, first);
	}

	template<typename T1, typename T2>
	std::pair<T2, T1> reversed_pair(const std::pair<T1, T2>& pair) {
		return std::make_pair(pair.second, pair.first);
	}
}

#endif /* NCV_PAIR_UTILS_H */