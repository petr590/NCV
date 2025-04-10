#include "debug.h"

namespace ncv {
	std::ofstream log("log.txt");

	void timer::finish() {
		if (start != 0) {
			clock_t end = clock();
			log << message << ' ' << float(end - start) * (1000.f / CLOCKS_PER_SEC) << " ms" << endl;
			start = 0;
		}
	}
}