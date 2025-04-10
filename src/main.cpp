#include "main.h"
#include "reader.h"
#include "drawer.h"
#include "double_buffer.h"
#include "interthread.h"
#include <chrono>
#include <future>

#include "debug.h"

namespace ncv {
	namespace this_thread = std::this_thread;
	using namespace std::chrono_literals;
	using std::vector;
	using std::ref;
	using std::async;
	using std::launch;
	using std::future;
	using std::future_status;
	using std::lock_guard;
	using std::mutex;

	static bool needRedraw = false;

	static void readFromConsole(const vector<File>& files, size_t& index) {
		for (;;) {
			lock_guard<mutex> lock(ncursesMutex);

			switch (getch()) {
				case KEY_LEFT: case 'a':
					index = (index + files.size() - 1) % files.size();
					break;
				
				case KEY_RIGHT: case 'd':
					index = (index + 1) % files.size();
					break;
				
				case 'r':
					needRedraw = true;
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

			this_thread::yield();
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


	static void start(future<void>& readThread, future<void>& drawThread, future<void>& dbufThread,
					  const vector<File>& files, size_t index) {
		stopped = false;
		readThread = startFuture(readFrameGroups, ref(files[index]));
		drawThread = startFuture(drawFrameGroups, ref(files[index]), ref(readThread));
		dbufThread = startFuture(renderDoubleBuffered, ref(readThread));
	}


	static void stop(future<void>& readThread, future<void>& drawThread, future<void>& dbufThread) {
		stopped = true;
		readThread.wait();
		drawThread.wait();
		dbufThread.wait();
	}


	void run(const vector<File>& files, size_t index) {
		drawBuffer1 = newwin(LINES, COLS, 0, 0);
		drawBuffer2 = newwin(LINES, COLS, 0, 0);

		future<void> readThread, drawThread, dbufThread;
		start(readThread, drawThread, dbufThread, files, index);

		future<void> consoleThread = startFuture(readFromConsole, ref(files), ref(index));
		size_t oldIndex = index;

		while (consoleThread.wait_for(0ms) != future_status::ready) {
			if (exPtr != nullptr) {
				std::rethrow_exception(exPtr);
			}

			if (needRedraw) {
				stop(readThread, drawThread, dbufThread);
				start(readThread, drawThread, dbufThread, files, index);
				needRedraw = false;
			}

			if (oldIndex != index) {
				oldIndex = index;

				stop(readThread, drawThread, dbufThread);
				start(readThread, drawThread, dbufThread, files, index);
			}

			this_thread::yield();
		}

		stop(readThread, drawThread, dbufThread);
	}
}
