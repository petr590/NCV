#ifndef NCV_DRAWER_H
#define NCV_DRAWER_H

#include "files.h"
#include <future>

namespace ncv {
	/* Отрисовывает группы фреймов */
	void drawFrameGroups(const File&, const std::future<void>& readThread);
}

#endif /* NCV_DRAWER_H */