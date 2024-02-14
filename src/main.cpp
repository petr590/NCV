#include "image_entry.cpp"
#include "files.cpp"
#include "default_colors.cpp"
#include "alert.h"
#include <iostream>
#include <csignal>

namespace ncv {
	using std::tie;
	using std::to_wstring;

	using std::cerr;
	using std::endl;


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
		for (int y = sy; y < ey; ++y) {
			move(y, sx);

			for (int x = sx; x < ex; ++x) {
				addch(' ');
			}
		}

		if (image.success()) {
			attrset(0);
			image.draw(sx, sy, ex, ey);
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
				
				case 'b':
					redraw(images[index], sx, sy, ex, ey);

					toggleBigCharsMode();

					tie(sx, sy, ex, ey) = realert();

					break;
				
				case 'w':
					redraw(images[index], sx, sy, ex, ey);
					resetAlerted();
					
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
					resetAlerted();

					if (shown != HELP) {
						tie(sx, sy, ex, ey) = alert(
							L"-> | D - следующее изображение" LN
							L"<- | A - предыдущее изображение" LN
							L"W - информация о текущем изображении" LN
							L"S - добавить/убрать шум" LN
							L"E - использовать 2.5 символа вместо 2 на пиксель " LN
							L"R - перерисовать изображение" LN
							L"B - включить/выключить большие символы" LN
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
				resetAlerted();
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
