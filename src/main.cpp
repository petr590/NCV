#include "image_entry.cpp"
#include "files.cpp"
#include "default_colors.cpp"
#include <iostream>
#include <csignal>

namespace ncv {
	using std::tuple;
	using std::tie;
	
	using std::to_wstring;

	using std::cerr;
	using std::endl;


	tuple<int, int, int, int> alert(const wstring& message) {
		int width = 0,
			height = 0;

		int lineWidth = 0,
			maxLineWidth = COLS - 2;
		
		vector<pair<int, int>> breaks;

		for (size_t p = 0, i = 0, size = message.size(); i <= size; ++i) {
			wchar_t ch = message[i];

			bool isEol = ch == L'\n' || ch == L'\0';

			if (!isEol) {
				lineWidth += 1;
			}

			if (isEol || lineWidth == maxLineWidth) {
				width = max(width, lineWidth);

				lineWidth = 0;
				height += 1;

				size_t np = isEol ? i - 1 : i;
				breaks.emplace_back(p, np);
				p = i + 1;
			}
		}

		int tx = max((COLS - width) / 2, 1),
			ty = max((LINES - height) / 2, 1);

		int sx = tx - 1,
			sy = ty - 1,
			ex = tx + width,
			ey = ty + height;


		{
			int y = ty;
			for (const auto& entry : breaks) {
				int l = entry.second - entry.first + 1;

				mvaddnwstr(y, tx, message.c_str() + entry.first, l);

				for (l += tx; l < ex; ++l) {
					addch(' ');
				}
			}
		}

		
		mvaddch(sy, sx, ACS_ULCORNER);
		mvaddch(sy, ex, ACS_URCORNER);
		mvaddch(ey, sx, ACS_LLCORNER);
		mvaddch(ey, ex, ACS_LRCORNER);

		for (int x = tx; x < ex; ++x) {
			mvaddch(sy, x, ACS_HLINE);
			mvaddch(ey, x, ACS_HLINE);
		}

		for (int y = ty; y < ey; ++y) {
			mvaddch(y, sx, ACS_VLINE);
			mvaddch(y, ex, ACS_VLINE);
		}

		return {sx, sy, ex, ey};
	}


	void draw(vector<ImageEntry>& images, const vector<File>& files, size_t index) {
		
		clear();

		ImageEntry& image = images[index];

		if (image.empty()) {
			image.load(files[index].path().string().c_str());
		}

		if (!image.success()) {
			resetColors();
			alert(L"Cannot load image \"" + files[index].wpath() + L'"');

		} else {
			rgb_t background = defaultColors[0].toRgb();

			// Провести ресайзинг и квантизацию если размеры окна изменились
			image.process(screenWidthPixels(), screenHeightPixels(), COLORS, background);

			image.initColors(background);
			image.draw();
		}

		refresh();
	}


	void redraw(ImageEntry& image, int sx, int sy, int ex, int ey) {
		if (image.success()) {
			attrset(0);

			for (int y = sy; y <= ey; ++y) {
				move(y, sx);

				for (int x = sx; x <= ex; ++x) {
					addch(' ');
				}
			}

			image.draw(sx, sy, ex + 1, ey + 1);

		} else {
			for (int y = sy; y <= ey; ++y) {
				move(y, sx);

				for (int x = sx; x <= ex; ++x) {
					addch(' ');
				}
			}
		}
	}


	#define LN L"\n"

	enum Shown {
		NONE,
		HELP,
		FILE_INFO
	};


	void run(const vector<File>& files, size_t index) {
		vector<ImageEntry> images(files.size());

		draw(images, files, index);

		int lines = LINES,
			cols = COLS;
		
		Shown shown = NONE;
		
		int sx = 0, sy = 0,
			ex = 0, ey = 0;

		for (;;) {
			size_t oldIndex = index;
			bool needRedraw = false;

			switch (getch()) {
				case KEY_LEFT: case 'a':
					index = (index + files.size() - 1) % files.size();
					break;
				
				case KEY_RIGHT: case 'd':
					index = (index + 1) % files.size();
					break;
				
				case 's':
					images[index].toggleNoise();
					needRedraw = true;
					break;
				
				case 'e':
					squeezed = !squeezed;
					needRedraw = true;
					break;
				
				case 'r':
					needRedraw = true;
					break;
				
				case 'w':
					redraw(images[index], sx, sy, ex, ey);
					
					if (shown != FILE_INFO) {
						tie(sx, sy, ex, ey) = alert(
							L"File: " + files[index].wpath() + LN
							L"Size: " + to_wstring(files[index].size()) + L" B" LN
							L"Colors: " + to_wstring(images[index].getColorTable().size()) +
								L" / " + to_wstring(COLORS) +
							(images[index].quantized() ? LN L"quantized" : LN L"not quantized")
						);

						shown = FILE_INFO;

					} else {
						sx = sy = ex = ey = 0;
						shown = NONE;
					}

					break;
				
				case KEY_F(1):
					redraw(images[index], sx, sy, ex, ey);

					if (shown != HELP) {
						tie(sx, sy, ex, ey) = alert(
							L"-> | D - следующее изображение" LN
							L"<- | A - предыдущее изображение" LN
							L"W - информация о текущем изображении" LN
							L"S - добавить/убрать шум" LN
							L"E - использовать 2.5 символа вместо 2 на пиксель" LN
							L"R - перерисовать изображение" LN
							L"Q | Esc - выход"
						);

						shown = HELP;

					} else {
						sx = sy = ex = ey = 0;
						shown = NONE;
					}

					break;
				
				case 27: {
					nodelay(stdscr, true);
					int ch = getch();
					nodelay(stdscr, false);

					if (ch == -1) { // Escape
						return;
					}

					break;
				}
				
				case 'q':
					return;
			}
			
			if (lines != LINES || cols != COLS || oldIndex != index || needRedraw) {
				draw(images, files, index);
				lines = LINES;
				cols = COLS;
				shown = NONE;
			}
		}
	}


	using std::signal;

	void onError(int signum) {
		if (!isendwin()) {
			ncurses_end();
		}

		signal(signum, SIG_DFL);
	}
}


int main(int argc, const char* args[]) {
	using namespace ncv;

	setlocale(LC_ALL, "");

	srand(time(NULL));


	signal(SIGSEGV, onError);
	signal(SIGABRT, onError);


	string fileOrDir;

	switch (argc) {
		case 1:
			fileOrDir = ".";
			break;
		
		case 2:
			fileOrDir = args[1];

			if (!fs::exists(fileOrDir)) {
				cerr << "No such file or directory: \"" << fileOrDir << '"' << endl;
				return 1;
			}

			break;
		
		default:
			cerr << "Usage: " << args[0] << " [image|directory]" << endl;
			return 1;
	}


	initscr();

	if (!has_colors() || !can_change_color()) {
		endwin();
		cerr << "Your terminal does not support variable colors" << endl;
		return 2;
	}

	start_color();
	initDefaultColors();


	vector<File> files;
	size_t startIndex;

	tie(files, startIndex) = findFiles(fileOrDir);


	if (files.empty()) {
		endwin();
		cerr << "No images found in directory \"" << fileOrDir << '"' << endl;
		return 1;
	}

	ncurses_start();

	run(files, startIndex);

	ncurses_end();
	
	return 0;
}
