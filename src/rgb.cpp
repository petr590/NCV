#ifndef NCV_RGB_CPP
#define NCV_RGB_CPP

#include <cmath>
#include <cstdint>

namespace ncv {
#	if 1
	typedef int rgb_t;
#	else

	struct rgb_t {
		int v;

		rgb_t(int v): v(v) {}

		inline int operator>>(int x) {
			return v >> x;
		}

		friend inline int operator&(rgb_t rgb, int x) {
			return rgb.v & x;
		}
	};

#	endif

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

#endif /* NCV_RGB_CPP */
