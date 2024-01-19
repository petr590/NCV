#ifndef NCV_RGB_CPP
#define NCV_RGB_CPP

#include <cmath>
#include <cstdint>

namespace ncv {
	inline int getR(int color) {
		return (color >> 16) & 0xFF;
	}
	
	inline int getG(int color) {
		return (color >> 8) & 0xFF;
	}
	
	inline int getB(int color) {
		return color & 0xFF;
	}

	inline int getColor(int r, int g, int b) {
		return	((r & 0xFF) << 16) |
				((g & 0xFF) <<  8) |
				 (b & 0xFF);
	}
}

#endif /* NCV_RGB_CPP */
