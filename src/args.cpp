#include "args.h"
#include "error_codes.h"
#include <map>
#include <string>
#include <functional>
#include <filesystem>

extern "C" {
	#include <libavutil/log.h>
}

namespace ncv {
	bool bigCharsMode = false;
	bool doubleResolution = false;
	bool parallel = false;
	

	#define RN "\r\n"
	
	namespace fs = std::filesystem;
	using std::map;
	using std::string;
	using std::function;

	class ArgParser {
		map<string, function<void(const char*[], int)>> callbacks;

		void addCallback(const string& n1, const string& n2, const function<void(const char*[], int)>& callback) {
			callbacks[n1] = callbacks[n2] = callback;
		}

		void addCallback(const string& n1, const string& n2, const function<void()>& callback) {
			callbacks[n1] = callbacks[n2] = [callback] (const char*[], int) { callback(); };
		}

	public:
		ArgParser() {
			addCallback("-b", "--big-chars", [] () { bigCharsMode     = true; });
			addCallback("-e", "--extended",  [] () { doubleResolution = true; });
			addCallback("-p", "--parallel",  [] () { parallel         = true; });

			addCallback("-h", "--help", [] (const char* argv[], int) {
				av_log(nullptr, AV_LOG_INFO,
					"Usage: %s [options] [file|directory]" RN
					"Options:" RN
					"-b, --big-chars:  use big chars (for small console scale)" RN
					"-e, --extended:   use double resolution (may be slower, especially for video)" RN
					"-p, --parallel:   play video parallel to decoding (may cause lags)" RN
					"",
					argv[0]
				);

				exit(EXIT_SUCCESS);
			});
		}

		const char* parse(int argc, const char* argv[]) const {
			const char* fileOrDir = nullptr;

			for (int i = 1; i < argc; ++i) {
				string arg = argv[i];

				if (!arg.empty() && arg[0] == '-') {
					const auto callback = callbacks.find(arg);

					if (callback != callbacks.end()) {
						callback->second(argv, i);
					} else {
						av_log(nullptr, AV_LOG_ERROR, "Unknown optioin \"%s\"" RN, arg.c_str());
						exit(ARGUMENTS_ERROR);
					}

				} else {
					if (fileOrDir != nullptr) {
						av_log(nullptr, AV_LOG_ERROR, "Usage: %s [options] [image|directory]" RN, argv[0]);
						exit(ARGUMENTS_ERROR);
					}

					fileOrDir = argv[i];

					if (!fs::exists(fileOrDir)) {
						av_log(nullptr, AV_LOG_ERROR, "No such file or directory: \"%s\"" RN, fileOrDir);
						exit(FILES_NOT_FOUND_ERROR);
					}
				}
			}

			return fileOrDir != nullptr ? fileOrDir : ".";
		}
	};


	const char* parseArgs(int argc, const char* argv[]) {
		static ArgParser parser;
		return parser.parse(argc, argv);
	}
}
