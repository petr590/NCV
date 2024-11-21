#include "arg_parser.h"
#include "error_codes.h"
#include "ncurses_wrap.h"
#include "drawer.h"
#include "alert.h"
#include <map>
#include <functional>

extern "C" {
	#include <libavutil/log.h>
}

namespace ncv {
	#define RN "\r\n"
	
	using std::map;
	using std::string;
	using std::function;

	class ArgParser {
		map<string, function<void(const char*[], int)>> callbacks;

		void addCallback(const string& n1, const string& n2, function<void(const char*[], int)> callback) {
			callbacks[n1] = callbacks[n2] = callback;
		}

		void addCallback(const string& n1, const string& n2, function<void()> callback) {
			callbacks[n1] = callbacks[n2] = [callback] (const char*[], int) { callback(); };
		}

	public:
		ArgParser() {
			addCallback("-b", "--big-chars", [] () { setBigCharsMode(true); });
			addCallback("-e", "--extended",  [] () { doubleResolution = true; });
			addCallback("-p", "--parallel",  [] () { parallel = true; });

			addCallback("-h", "--help", [] (const char* args[], int) {
				av_log(NULL, AV_LOG_INFO,
					"Usage: %s [options] [file|directory]" RN
					"Options:" RN
					"-b, --big-chars:  use big chars (for small console scale)" RN
					"-e, --extended:   use double resolution (may be slower, especially for video)" RN
					"-p, --parallel:   play video parallel to decoding (may cause lags)" RN
					"",
					args[0]
				);

				exit(EXIT_SUCCESS);
			});
		}

		const char* parse(int argc, const char* args[]) const {
			const char* fileOrDir = nullptr;

			for (int i = 1; i < argc; ++i) {
				string arg = args[i];

				if (!arg.empty() && arg[0] == '-') {
					const auto callback = callbacks.find(arg);

					if (callback != callbacks.end()) {
						callback->second(args, i);
					} else {
						av_log(NULL, AV_LOG_ERROR, "Unknown optioin \"%s\"" RN, arg.c_str());
						exit(ARGUMENTS_ERROR);
					}

				} else {
					if (fileOrDir != nullptr) {
						av_log(NULL, AV_LOG_ERROR, "Usage: %s [options] [image|directory]" RN, args[0]);
						exit(ARGUMENTS_ERROR);
					}

					fileOrDir = args[i];

					if (!fs::exists(fileOrDir)) {
						av_log(NULL, AV_LOG_ERROR, "No such file or directory: \"%s\"" RN, fileOrDir);
						exit(FILES_NOT_FOUND_ERROR);
					}
				}
			}

			return fileOrDir != nullptr ? fileOrDir : ".";
		}
	};


	const char* parseArgs(int argc, const char* args[]) {
		static ArgParser parser;
		return parser.parse(argc, args);
	}
}