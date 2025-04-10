#include "ncurses_wrap.h"
#include "args.h"
#include <cstdlib>

void ncurses_start() {
	curs_set(false);
	noecho();
	keypad(stdscr, true);
	timeout(0);

	#ifndef NO_USE_SET_ESCDELAY
	// Убираем задержку при нажатии Esc
	// Если этой функции нет в вашей версии ncurses, отключите её с помощью макроса NO_USE_SET_ESCDELAY
	set_escdelay(0);
	#endif
}

void ncurses_end() {
	clear();
	refresh();

	keypad(stdscr, false);
	echo();
	curs_set(true);
	endwin();
}


#ifdef NEED_MVADDNWSTR_IMPL

int mvaddnwstr(int y, int x, const wchar_t* wstr, int n) {
	char* str = new char[n * sizeof(wchar_t) + 1];
	int l = wcstombs(str, wstr, n);
	int res = mvaddnstr(y, x, str, l);
	delete[] str;
	return res;
}

#undef NEED_MVADDNWSTR_IMPL

#endif


namespace ncv {
	using std::min;

	const float
			DEFAULT_X_SCALE = 2,
			DEFAULT_Y_SCALE = 1,
			DOUBLE_X_SCALE = DEFAULT_X_SCALE / 2,
			DOUBLE_Y_SCALE = DEFAULT_Y_SCALE / 2;

	float scaleX() {
		return doubleResolution ? DOUBLE_X_SCALE : DEFAULT_X_SCALE;
	}

	float scaleY() {
		return doubleResolution ? DOUBLE_Y_SCALE : DEFAULT_Y_SCALE;
	}

	int screenWidthPixels() {
		return static_cast<int>(COLS / scaleX());
	}

	int screenHeightPixels() {
		return static_cast<int>(LINES / scaleY());
	}

	int viewportWidthPixels() {
		return screenWidthPixels();
	}

	int viewportHeightPixels() {
		return static_cast<int>((LINES - 1) / scaleY()); // Резервируем место под строку состояния
	}

	std::pair<int, int> fitSize(int scrWidth, int scrHeight, int imgWidth, int imgHeight) {
		
		double ratio = min(min(
			static_cast<double>(scrWidth) / imgWidth,
			static_cast<double>(scrHeight) / imgHeight
		), 1.0);
		
		return {
			static_cast<int>(round(imgWidth * ratio)),
			static_cast<int>(round(imgHeight * ratio))
		};
	}
}
