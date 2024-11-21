#include "main.h"
#include "arg_parser.h"
#include "files.h"
#include "throw_if_error.h"
#include "default_colors.cpp"
#include "reader.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

#include <filesystem>
#include <csignal>

#include "debug.h"

namespace ncv {

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

	const char* fileOrDir = parseArgs(argc, args);


	initscr();

	if (!has_colors() || !can_change_color()) {
		endwin();
		av_log(NULL, AV_LOG_ERROR, "Your terminal does not support variable colors\r\n");
		return COLORS_NOT_SUPPORTED_ERROR;
	}

	start_color();
	initDefaultColors();
	ncurses_start();


	vector<File> files;
	size_t startIndex;

	tie(files, startIndex) = findFiles(fileOrDir);


	if (files.empty()) {
		endwin();
		av_log(NULL, AV_LOG_ERROR, "No media files found in directory \"%s\"\r\n", fileOrDir);
		return FILES_NOT_FOUND_ERROR;
	}


	// Отключить логи и предупреждения из libav, так как мы отрисовываем
	// изображения в терминале, и вывод в него будет ломать изображение
	av_log_set_callback([] (void* avcl, int level, const char* fmt, va_list args) {
		if (level == AV_LOG_PANIC ||
			level == AV_LOG_FATAL ||
			level == AV_LOG_ERROR) {
			
			av_log_default_callback(avcl, level, fmt, args);
		}
	});


	run(files, startIndex);

	ncurses_end();


	return EXIT_SUCCESS;
}