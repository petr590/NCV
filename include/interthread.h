#ifndef NCV_INTERTHREAD_H
#define NCV_INTERTHREAD_H

#include "frame_group.h"
#include <mutex>
#include <deque>

namespace ncv {
	/* Мьютекс для ncurses */
	extern std::mutex ncursesMutex;

	/* Мьютекс для frameGroups */
	extern std::mutex frameGroupsMutex;

	/* Очередь групп на отрисовку. Доступ к этой очереди всегда
	 * должен осуществляться с блокированием frameGroupsMutex */
	extern std::deque<FrameGroup> frameGroups;

	/* Сколько процентов видеопотока уже прочитано */
	extern int frameGroupsProcessPercent;

	/* Если true, то потоки чтения, отрисовки и двойной буферизации должны завершить работу */
	extern bool stopped;

	/* Буферы отрисовки для двойной буферизации. Меняются между собой каждый кадр.
	 * Доступ должен быть синхронизирован с помощью ncursesMutex */
	extern WINDOW *drawBuffer1, *drawBuffer2;

	/* Нужно ли обновить stdscr */
	extern bool updateStdscr;

	// /* Проверяет ввод каких-либо символов с консоли.
	//  * Вызывается синхронно из потока отрисовки для избегания параллельного
	//  * использования ncurses, что может привести к невалидному состоянию. */
	// extern void checkConsoleInput();

}

#endif /* NCV_INTERTHREAD_H */