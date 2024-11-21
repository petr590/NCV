#ifndef NCV_NCURSES_WRAP_H
#define NCV_NCURSES_WRAP_H

#include "int_pair.h"
#include <ncurses.h>
#include <cmath>

extern void ncurses_start();

extern void ncurses_end();


#ifndef mvaddnwstr // Если ncurses не работает с wchar_t

extern int mvaddnwstr(int y, int x, const wchar_t* wstr, int n);

#define mvaddnwstr mvaddnwstr
#define NEED_MVADDNWSTR_IMPL

#endif

#define EXT_COLOR_MASK 0xFFFF
#define EXT_COLOR_PAIR(p) ((p) & EXT_COLOR_MASK)


namespace ncv {
	extern bool doubleResolution;

	float scaleX();
	float scaleY();

	int screenWidthPixels();
	int screenHeightPixels();

	int viewportWidthPixels();
	int viewportHeightPixels();

	int_pair fitSize(int scrWidth, int scrHeight, int imgWidth, int imgHeight);
}


#endif /* NCV_NCURSES_WRAP_H */