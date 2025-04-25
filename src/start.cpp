#include "main.h"
#include "args.h"
#include "ncurses_wrap.h"
#include "error_codes.h"
#include <csignal>

extern "C" {
	#include <libavutil/log.h>
}

#include "debug.h"

namespace ncv {
	using std::signal;

	static const uint8_t SIGNALS[] = { SIGINT, SIGQUIT, SIGABRT, SIGFPE, SIGSEGV, SIGTERM };

	void on_signal(int) {
		exit(EXIT_SUCCESS);
	}

	void add_signal_handlers() {
		struct sigaction action = {};
		action.sa_handler = on_signal;
		
		sigemptyset(&action.sa_mask);

		for (int sig : SIGNALS) {
			sigaddset(&action.sa_mask, sig);
		}

		for (int sig : SIGNALS) {
			sigaction(sig, &action, nullptr);
		}
	}
}

int main(int argc, const char* argv[]) {
	using namespace ncv;
	using std::vector;
	using std::tie;

	setlocale(LC_ALL, "");
	srand(time(nullptr));

	add_signal_handlers();

	atexit([] () {
		if (!isendwin()) {
			ncurses_end();
		}
	});


	const char* fileOrDir = parseArgs(argc, argv);

	initscr();

	if (!has_colors() || !can_change_color()) {
		endwin();
		av_log(nullptr, AV_LOG_ERROR, "Your terminal does not support variable colors\r\n");
		return COLORS_NOT_SUPPORTED_ERROR;
	}

	start_color();
	ncurses_start();


	vector<File> files;
	size_t startIndex;

	tie(files, startIndex) = findFiles(fileOrDir);


	if (files.empty()) {
		endwin();
		av_log(nullptr, AV_LOG_ERROR, "No media files found in directory \"%s\"\r\n", fileOrDir);
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
	return EXIT_SUCCESS;
}
