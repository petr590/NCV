#ifndef NCV_DOUBLE_BUF_H
#define NCV_DOUBLE_BUF_H

#include <future>

namespace ncv {
	void renderDoubleBuffered(const std::future<void>& readThread);
}

#endif /* NCV_DOUBLE_BUF_H */
