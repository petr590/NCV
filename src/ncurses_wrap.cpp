#ifndef NCV_NCURSES_WRAP_CPP
#define NCV_NCURSES_WRAP_CPP

#include <ncurses.h>

void ncurses_start() {
	curs_set(false);
	noecho();
	keypad(stdscr, true);

	#ifndef NO_USE_SET_ESCDELAY
	// Убираем задержку при нажатии Esc
	set_escdelay(0); // Если этой функции нет в вашей версии ncurses, отключите её с помощью макроса
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


#ifndef mvaddnwstr // Если ncurses не работает с wchar_t

int mvaddnwstr(int y, int x, const wchar_t* wstr, int n) {
	char* str = new char[n * sizeof(wchar_t) + 1];
	int l = wcstombs(str, wstr, n);
	int res = mvaddnstr(y, x, str, l);
	delete str;
	return res;
}

#define mvaddnwstr mvaddnwstr

#endif


namespace ncv {

	float DEFAULT_X_SCALE  = 2.0f,
		  SQUEEZED_X_SCALE = 2.5f;

	bool squeezed = false;

	inline float scaleX() {
		return squeezed ? SQUEEZED_X_SCALE : DEFAULT_X_SCALE;
	}

	inline int screenWidthPixels() {
		return static_cast<int>(COLS / scaleX());
	}

	inline int screenHeightPixels() {
		return LINES;
	}
}


#endif /* NCV_NCURSES_WRAP_CPP */