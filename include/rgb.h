#ifndef NCV_RGB_H
#define NCV_RGB_H

#include <tuple>

namespace ncv {
#	if 1
	using rgb_t      = int; // Цвет
	using index_t    = int; // Индекс цвета
	using cp_index_t = int; // Индекс цветовой пары
	using count_t    = int; // Количество чего-либо
#	else

	#define STRUCT(name) struct name {\
		int v;\
		name(int v): v(v) {}\
		inline int operator>>(int x) const { return v >> x; }\
		inline bool operator<(const name& n) const { return v < n.v; }\
		inline bool operator>(const name& n) const { return v > n.v; }\
		inline bool operator==(const name& n) const { return v == n.v; }\
		inline bool operator!=(const name& n) const { return v != n.v; }\
		friend inline int operator&(const name& n, int x) { return n.v & x; }\
		\
		friend inline const name& operator+(const name& n1, const name& n2) { return name(n1.v + n2.v); }\
		friend inline const name& operator-(const name& n1, const name& n2) { return name(n1.v - n2.v); }\
		friend inline const name& operator*(const name& n1, const name& n2) { return name(n1.v * n2.v); }\
		friend inline const name& operator/(const name& n1, const name& n2) { return name(n1.v / n2.v); }\
		\
		inline name& operator+=(const name& n) { v += n.v; return *this; }\
		inline name& operator-=(const name& n) { v -= n.v; return *this; }\
	}

	STRUCT(rgb_t);
	STRUCT(index_t);
	STRUCT(cp_index_t);
	STRUCT(count_t);
#	endif

	using index_pair = std::pair<index_t, index_t>;

	const rgb_t RGB_NONE = -1;


	const float
			RM = 0.299f,
			GM = 0.587f,
			BM = 0.114f,
			BREIGHTNESS_TRESHOLD = 100.f;
	

	inline int getR(rgb_t color) {
		return (color >> 16) & 0xFF;
	}
	
	inline int getG(rgb_t color) {
		return (color >> 8) & 0xFF;
	}
	
	inline int getB(rgb_t color) {
		return color & 0xFF;
	}

	inline rgb_t getColor(int r, int g, int b) {
		return rgb_t(
				((r & 0xFF) << 16) |
				((g & 0xFF) <<  8) |
				 (b & 0xFF)
		);
	}

	inline float brightness(rgb_t color) {
		return getR(color) * RM + getG(color) * GM + getB(color) * BM;
	}
}

#endif /* NCV_RGB_H */
