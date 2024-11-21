#include "interthread.h"

namespace ncv {
	std::mutex frameGroupsMutex {};
	std::deque<FrameGroup> frameGroups {};

	bool stopped = false;
}