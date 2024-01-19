#include "image_entry.cpp"
#include <string>
#include <iostream>
#include <filesystem>


namespace ncv {
	using std::string;

	inline bool stringEndsWith(const string& str, const string& ending) {
		size_t	ssize = str.size(),
				esize = ending.size();
		return ssize >= esize && str.compare(ssize - esize, esize, ending) == 0;
	}


	namespace fs = std::filesystem;

	struct ColorContent {
		short r, g, b;

		ColorContent() {}

		ColorContent(int index) {
			color_content(index, &r, &g, &b);
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
		for (int i = 0, size = defaultColors.size(); i < size; ++i) {
			init_color(i, defaultColors[i].r, defaultColors[i].g, defaultColors[i].b);
		}

		for (int i = 0, size = defaultColorPairs.size(); i < size; ++i) {
			init_pair(i, defaultColorPairs[i].fg, defaultColorPairs[i].bg);
		}
	}
}


int main(int argc, const char* args[]) {
	using namespace ncv;

	srand(time(NULL));


	string directory;

	switch (argc) {
		case 1:
			directory = ".";
			break;
		
		case 2:
			directory = args[1];

			if (!fs::exists(directory)) {
				cerr << "No such file or directory: \"" << directory << '"' << endl;
				return 1;
			}

			break;
		
		default:
			cerr << "Usage: " << args[0] << " <image>" << endl;
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


	const string extensions[] = { ".png", ".jpg", ".gif", ".bmp", ".psd", ".pic", ".pnm" };


	vector<string> files;
	int index = 0;
	string firstFile;

	if (!fs::is_directory(directory)) {
		firstFile = basename(directory.c_str());
	
		directory = directory.substr(0, directory.size() - firstFile.size());
		
		if (directory.empty())
			directory = ".";
	}


	for (const auto& entry : fs::directory_iterator(directory)) {
		const string& path = entry.path();
		
		std::error_code ec; // Ignore errors

		if (fs::is_regular_file(path, ec)) {
			if (!firstFile.empty() && basename(path.c_str()) == firstFile) {
				index = files.size();
				files.push_back(path);
				continue;
			}

			for (const string& extension : extensions) {
				if (stringEndsWith(path, extension)) {
					files.push_back(path);
					break;
				}
			}
		}
	}


	if (files.empty()) {
		endwin();
		cerr << "No images found in directory \"" << directory << '"' << endl;
		return 1;
	}


	vector<ImageEntry> images(files.size());

	curs_set(false);
	noecho();
	keypad(stdscr, true);
	set_escdelay(0);


	for (;;) {
		clear();

		ImageEntry& image = images[index];

		if (image.empty()) {
			image.load(files[index].c_str());
		}

		if (!image.success()) {
			resetColors();

			string message = "Cannot load image \"" + files[index] + "\"";
			
			int x = (COLS - message.size()) / 2,
				y = LINES / 2;
			
			mvaddstr(y, x, message.c_str());

		} else {
			// Провести ресайзинг и квантизацию если в первый раз размеры окна изменились
			image.process(COLS / 2, LINES, COLORS - 1 /* Индекс 0 зарезервирован */);

			const vector<int>& colorTable = image.getColorTable();

			for (int index = 0; index < colorTable.size(); ) {
				int color = colorTable[index++];

				init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);
				init_pair(index, index, index);
			}

			image.draw();
		}

		refresh();

		switch (getch()) {
			case KEY_LEFT: case 'a':
				index = (index + files.size() - 1) % files.size();
				continue;
			
			case KEY_RIGHT: case 'd':
				index = (index + 1) % files.size();
				continue;
			
			case 's':
				images[index].toggleNoise();
				continue;
			
			default:
				continue;
			
			case 27: {
				nodelay(stdscr, true);
				int ch = getch();
				nodelay(stdscr, false);

				if (ch == -1) {
					break;
				}

				continue;
			}
			
			case 'q':
				break;
		}

		break;
	}
	
	keypad(stdscr, false);
	echo();
	curs_set(true);
	endwin();
	
	return 0;
}
