#ifndef NCV_NCURSES_WRAP_H
#define NCV_NCURSES_WRAP_H

#include <ncurses.h>
#include <cmath>
#include <tuple>

void ncurses_start();

void ncurses_end();


#ifndef mvaddnwstr // Если ncurses не работает с wchar_t

int mvaddnwstr(int y, int x, const wchar_t* wstr, int n);

#define mvaddnwstr mvaddnwstr
#define NEED_MVADDNWSTR_IMPL

#endif

#define EXT_COLOR_MASK 0xFFFF
#define EXT_COLOR_PAIR(p) ((p) & EXT_COLOR_MASK)


namespace ncv {
	float scaleX();
	float scaleY();

	int screenWidthPixels();
	int screenHeightPixels();

	int viewportWidthPixels();
	int viewportHeightPixels();

	std::pair<int, int> fitSize(int scrWidth, int scrHeight, int imgWidth, int imgHeight);
}


#endif /* NCV_NCURSES_WRAP_H */