#include "rgb.h"
#include "ncurses_wrap.h"
#include <vector>

namespace ncv {
	using std::vector;

	struct ColorContent {
		short r, g, b;

		ColorContent() {}

		ColorContent(int index) {
			color_content(index, &r, &g, &b);
		}

		rgb_t toRgb() const {
			return getColor(r * 255 / 1000, g * 255 / 1000, b * 255 / 1000);
		}
	};

	struct PairContent {
		short fg, bg;

		PairContent() {}

		PairContent(int index) {
			pair_content(index, &fg, &bg);
		}
	};


	vector<ColorContent> defaultColors;
	vector<PairContent> defaultColorPairs;

	void initDefaultColors() {
		defaultColors.resize(COLORS);
		defaultColorPairs.resize(COLOR_PAIRS);

		for (int i = 0; i < COLORS; ++i) {
			defaultColors[i] = ColorContent(i);
		}

		for (int i = 0; i < COLOR_PAIRS; ++i) {
			defaultColorPairs[i] = PairContent(i);
		}
	}

	void resetColors() {
		for (size_t i = 0, size = defaultColors.size(); i < size; ++i) {
			init_color(static_cast<short>(i), defaultColors[i].r, defaultColors[i].g, defaultColors[i].b);
		}

		for (size_t i = 0, size = defaultColorPairs.size(); i < size; ++i) {
			init_pair(static_cast<short>(i), defaultColorPairs[i].fg, defaultColorPairs[i].bg);
		}
	}
}