#ifndef NCV_NCURSES_WRAP_CPP
#define NCV_NCURSES_WRAP_CPP

#include "ncurses_wrap.h"
#include <cstdlib>

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


#ifdef NEED_MVADDNWSTR_IMPL

int mvaddnwstr(int y, int x, const wchar_t* wstr, int n) {
	char* str = new char[n * sizeof(wchar_t) + 1];
	int l = wcstombs(str, wstr, n);
	int res = mvaddnstr(y, x, str, l);
	delete str;
	return res;
}

#undef NEED_MVADDNWSTR_IMPL

#endif


namespace ncv {
	bool squeezed = false;
}


#endif /* NCV_NCURSES_WRAP_CPP */