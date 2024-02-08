#ifndef NCV_INT_PAIR_CPP
#define NCV_INT_PAIR_CPP

namespace ncv {
	typedef long long int_pair;

	inline int_pair intPair(int first, int second) {
		return static_cast<int_pair>(first) << 24 | second;
	}

	inline int_pair sortedIntPair(int first, int second) {
		return first < second ?
				(static_cast<int_pair>(first) << 24 | second) :
				(static_cast<int_pair>(second) << 24 | first);
	}

	inline int_pair reverseIntPair(int_pair pair) {
		return ((pair << 24) & 0xFFFFFF000000LL) | ((pair >> 24) & 0xFFFFFFLL);
	}

	inline int firstInt(int_pair pair) {
		return static_cast<int>(pair >> 24) & 0xFFFFFF;
	}

	inline int secondInt(int_pair pair) {
		return static_cast<int>(pair) & 0xFFFFFF;
	}
}

#endif /* NCV_INT_PAIR_CPP */