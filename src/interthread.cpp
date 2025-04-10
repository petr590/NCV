#include "interthread.h"

namespace ncv {
	std::mutex ncursesMutex {};
	std::mutex frameGroupsMutex {};
	std::deque<FrameGroup> frameGroups {};
	bool stopped = false;
	WINDOW* drawBuffer1 = nullptr;
	WINDOW* drawBuffer2 = nullptr;
	bool updateStdscr = false;
}