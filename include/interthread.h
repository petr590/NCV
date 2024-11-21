#ifndef NCV_INTERTHREAD_H
#define NCV_INTERTHREAD_H

#include "frame_group.h"
#include <mutex>
#include <deque>

namespace ncv {
	/* Мьютекс для frameGroups */
	extern std::mutex frameGroupsMutex;

	/* Очередь групп на отрисовку. Доступ к этой очереди всегда
	 * должен осуществляться с блокированием frameGroupsMutex */
	extern std::deque<FrameGroup> frameGroups;

	/* Сколько процентов видеопотока уже прочитано */
	extern int frameGroupsProcessPercent;

	/* Если true, то потоки чтения и отрисовки должны завершить работу */
	extern bool stopped;
}

#endif /* NCV_INTERTHREAD_H */