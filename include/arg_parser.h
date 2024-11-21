#ifndef NCV_ARG_PARSER_H
#define NCV_ARG_PARSER_H

namespace ncv {
	/** Парсит аргументы командной строки.
	 * Возвращает путь к указанному пользователем файлу или "." если пользователь не указал файл.
	 * При неверных аргументах завершает выполнение программы. */
	const char* parseArgs(int argc, const char* args[]);
}

#endif /* NCV_ARG_PARSER_H */