#ifndef NCV_ARG_PARSER_H
#define NCV_ARG_PARSER_H

namespace ncv {
	/** Включен ли режим "больших символов" */
	extern bool bigCharsMode;

	/** Использовать ли двойное разрешение */
	extern bool doubleResolution;
	
	/* Если false, то ждёт полного декодирования потока.
	 * Если true, то выводит данные как только они будут декодированы. */
	extern bool parallel;
	
	/** Парсит аргументы командной строки.
	 * Возвращает путь к указанному пользователем файлу или "." если пользователь не указал файл.
	 * При неверных аргументах завершает выполнение программы. */
	const char* parseArgs(int argc, const char* argv[]);
}

#endif /* NCV_ARG_PARSER_H */