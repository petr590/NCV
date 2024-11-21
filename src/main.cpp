#include "main.h"
#include "reader.h"
#include "drawer.h"
#include "interthread.h"
#include <chrono>
#include <future>

#include "debug.h"

namespace ncv {
	
	namespace this_thread = std::this_thread;
	using std::vector;
	using std::ref;
	using std::async;
	using std::launch;
	using std::future;
	using std::future_status;

	static const std::chrono::milliseconds ZERO(0);

	static bool needRedraw = false;

	void readFromConsole(const vector<File>& files, size_t& index) {
		for (;;) {
			switch (getch()) {
				case KEY_LEFT: case 'a':
					index = (index + files.size() - 1) % files.size();
					break;
				
				case KEY_RIGHT: case 'd':
					index = (index + 1) % files.size();
					break;

				case 27: {
					nodelay(stdscr, true);
					int ch = getch();
					nodelay(stdscr, false);

					if (ch == -1) { // Escape
						return;
					}
				}
				
				case 'q':
					return;
			}
		}
	}


	static std::exception_ptr exPtr = nullptr;

	template<typename... Args, typename... WrappedArgs>
	static future<void> startFuture(void (&fn)(Args...), WrappedArgs... wrappedArgs) {
		return async(launch::async, [fn] (Args... args) {
			try {
				fn(args...);
			}
			
		#ifndef NDEBUG
			catch (const std::exception& ex) {
				log << "ERROR: " << ex.what() << endl;
				exPtr = std::current_exception();

			} catch (...) {
				log << "ERROR" << endl;
				exPtr = std::current_exception();
			}
		#else
			catch (...) {
				exPtr = std::current_exception();
			}
		#endif
		}, wrappedArgs...);
	}


	static void start(future<void>& readThread, future<void>& drawThread,
					  const vector<File>& files, size_t index) {
		stopped = false;
		readThread = startFuture(readFrameGroups, ref(files[index]));
		drawThread = startFuture(drawFrameGroups, ref(files[index]), ref(readThread));
	}


	static void stop(future<void>& readThread, future<void>& drawThread) {
		stopped = true;
		readThread.wait();
		drawThread.wait();
	}


	void run(const vector<File>& files, size_t index) {
		future<void> readThread, drawThread;
		start(readThread, drawThread, files, index);

		future<void> consoleThread = startFuture(readFromConsole, ref(files), ref(index));
		size_t oldIndex = index;

		while (consoleThread.wait_for(ZERO) != future_status::ready) {
			if (exPtr != nullptr) {
				std::rethrow_exception(exPtr);
			}

			if (needRedraw) {
				// TODO
			}

			if (oldIndex != index) {
				oldIndex = index;

				stop(readThread, drawThread);
				start(readThread, drawThread, files, index);
			}

			this_thread::yield();
		}

		stop(readThread, drawThread);
	}
}
