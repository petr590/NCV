#ifndef NCV_NCURSES_WRAP_H
#define NCV_NCURSES_WRAP_H

#include <ncurses.h>

extern void ncurses_start();

extern void ncurses_end();


#ifndef mvaddnwstr // Если ncurses не работает с wchar_t

extern int mvaddnwstr(int y, int x, const wchar_t* wstr, int n);

#define mvaddnwstr mvaddnwstr
#define NEED_MVADDNWSTR_IMPL

#endif


namespace ncv {

	const float
			DEFAULT_X_SCALE  = 2.0f,
			SQUEEZED_X_SCALE = 2.5f;

	extern bool squeezed;

	static inline float scaleX() {
		return squeezed ? SQUEEZED_X_SCALE : DEFAULT_X_SCALE;
	}

	static inline int screenWidthPixels() {
		return static_cast<int>(COLS / scaleX());
	}

	static inline int screenHeightPixels() {
		return LINES;
	}
}


#endif /* NCV_NCURSES_WRAP_H */