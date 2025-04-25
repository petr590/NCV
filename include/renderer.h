#ifndef NCV_RENDERER_H
#define NCV_RENDERER_H

#include "files.h"
#include <future>

namespace ncv {
	/* Отрисовывает группы фреймов */
	void renderFrameGroups(const File&, const std::future<void>& readThread);
}

#endif /* NCV_RENDERER_H */