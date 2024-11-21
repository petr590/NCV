#ifndef NCV_DRAWER_H
#define NCV_DRAWER_H

#include "files.h"
#include <future>

namespace ncv {
	/* Если false, то ждёт полного декодирования потока.
	 * Если true, то выводит данные как только они будут декодированы. */
	extern bool parallel;


	/* Отрисовывает группы фреймов */
	void drawFrameGroups(const File&, const std::future<void>& readThread);
}

#endif /* NCV_DRAWER_H */