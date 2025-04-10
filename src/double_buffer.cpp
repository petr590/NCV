#include "double_buffer.h"
#include "interthread.h"

#include <mutex>
#include <thread>

namespace ncv {
	namespace this_thread = std::this_thread;
	using std::mutex;
	using std::lock_guard;

	void renderDoubleBuffered(const std::future<void>& readThread) {
		readThread.wait();

		while (!stopped) {
			if (updateStdscr) {
				lock_guard<mutex> lock(ncursesMutex);
				wrefresh(drawBuffer2);
				updateStdscr = false;
			}

			this_thread::yield();
		}
	}
}