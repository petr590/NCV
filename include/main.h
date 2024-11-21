#ifndef NCV_MAIN_H
#define NCV_MAIN_H

#include "files.h"

namespace ncv {

	/**
	 * Воспроизводит текущий файл (изначально files[index])
	 * Создаёт фоновые потоки:
	 * - поток чтения файла
	 * - поток отрисовки видео
	 * - поток чтения ввода с консоли
	 * @param files список файлов для чтения
	 * @param index индекс первого файла
	 */
	void run(const std::vector<File>& files, size_t index);
}


#endif /* NCV_MAIN_H */