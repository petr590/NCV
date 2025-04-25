#include "alert.h"
#include "args.h"
#include "ncurses_wrap.h"
#include <vector>

namespace ncv {
	using std::size;
	using std::max;
	using std::to_string;
	using std::string;
	using std::wstring;
	using std::vector;
	using std::tuple;
	using std::pair;
	using std::logic_error;

	const int CHAR_HEIGHT = 7;


	using big_char = const wchar_t*[CHAR_HEIGHT];

	const big_char UNKNOWN_CHAR = {
		L"█▀▀▀▀▀▀█",
		L"█ ▄▀▀▄ █",
		L"█   ▄▀ █",
		L"█   ▄  █",
		L"█▄▄▄▄▄▄█",
		L"        ",
		L"        ",
	};

	const big_char EMPTY_CHAR = {
		L"",
		L"",
		L"",
		L"",
		L"",
		L"",
		L"",
	};


	const wchar_t BIG_ASCII_START = L' ';

	const big_char BIG_ASCII[] = {
		{
			L"      ",
			L"      ",
			L"      ",
			L"      ",
			L"      ",
			L"      ",
			L"      ",
		},
		{
			L"██",
			L"██",
			L"██",
			L"  ",
			L"██",
			L"  ",
			L"  ",
		},
		{
			L"██  ██",
			L"██  ██",
			L"██  ██",
			L"      ",
			L"      ",
			L"      ",
			L"      ",
		},
		{
			L" ██  ██ ",
			L"████████",
			L" ██  ██ ",
			L"████████",
			L" ██  ██ ",
			L"        ",
			L"        ",
		},
		{
			L"▄▄███▄▄",
			L"██ █   ",
			L"███████",
			L"   █ ██",
			L"▀▀███▀▀",
			L"       ",
			L"       ",
		},
		{
			L"██  ██",
			L"   ██ ",
			L"  ██  ",
			L" ██   ",
			L"██  ██",
			L"      ",
			L"      ",
		},
		{
			L"  ▄█▀   ",
			L"  ██   ▄",
			L"▄██████▀",
			L"██  ██  ",
			L"▀████▀  ",
			L"        ",
			L"        ",
		},
		{
			L"██",
			L"██",
			L"██",
			L"  ",
			L"  ",
			L"  ",
			L"  ",
		},
		{
			L" ██",
			L"██ ",
			L"██ ",
			L"██ ",
			L" ██",
			L"   ",
			L"   ",
		},
		{
			L"██ ",
			L" ██",
			L" ██",
			L" ██",
			L"██ ",
			L"   ",
			L"   ",
		},
		{
			L"      ",
			L"▄ ██ ▄",
			L" ████ ",
			L"▀ ██ ▀",
			L"      ",
			L"      ",
			L"      ",
		},
		{
			L"   ▄▄   ",
			L"   ██   ",
			L"████████",
			L"   ██   ",
			L"   ▀▀   ",
			L"        ",
			L"        ",
		},
		{
			L"  ",
			L"  ",
			L"  ",
			L" ▄",
			L"█▀",
			L"  ",
			L"  ",
		},
		{
			L"        ",
			L"        ",
			L"████████",
			L"        ",
			L"        ",
			L"        ",
			L"        ",
		},
		{
			L"  ",
			L"  ",
			L"  ",
			L"  ",
			L"██",
			L"  ",
			L"  ",
		},
		{
			L"    ██",
			L"   ██ ",
			L"  ██  ",
			L" ██   ",
			L"██    ",
			L"      ",
			L"      ",
		},
		{
			L" ██████ ",
			L"██  ████",
			L"██ ██ ██",
			L"████  ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L" ██",
			L"███",
			L" ██",
			L" ██",
			L" ██",
			L"   ",
			L"   ",
		},
		{
			L"██████ ",
			L"     ██",
			L" █████ ",
			L"██     ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"     ██",
			L" █████ ",
			L"     ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L"██   ██",
			L"██   ██",
			L"███████",
			L"     ██",
			L"     ██",
			L"       ",
			L"       ",
		},
		{
			L"███████",
			L"██     ",
			L"███████",
			L"     ██",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L"██      ",
			L"███████ ",
			L"██    ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"███████",
			L"     ██",
			L"    ██ ",
			L"   ██  ",
			L"   ██  ",
			L"       ",
			L"       ",
		},
		{
			L" █████ ",
			L"██   ██",
			L" █████ ",
			L"██   ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L" █████ ",
			L"██   ██",
			L" ██████",
			L"     ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L"  ",
			L"▄▄",
			L"▀▀",
			L"▄▄",
			L"▀▀",
			L"  ",
			L"  ",
		},
		{
			L"  ",
			L"▄▄",
			L"▀▀",
			L" ▄",
			L"█▀",
			L"  ",
			L"  ",
		},
		{
			L"    ███",
			L"  ███  ",
			L"███    ",
			L"  ███  ",
			L"    ███",
			L"       ",
			L"       ",
		},
		{
			L"        ",
			L"████████",
			L"        ",
			L"████████",
			L"        ",
			L"        ",
			L"        ",
		},
		{
			L"███    ",
			L"  ███  ",
			L"    ███",
			L"  ███  ",
			L"███    ",
			L"       ",
			L"       ",
		},
		{
			L"▄█████ ",
			L"     ██",
			L"  ▄███ ",
			L"  ▀▀   ",
			L"  ██   ",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L"██  ▄ ██",
			L"██ █▀ ██",
			L"██ █▄ ██",
			L" █ ▀███ ",
			L"        ",
			L"        ",
		},
		{
			L" █████ ",
			L"██   ██",
			L"███████",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L" ██████",
			L"██     ",
			L"██     ",
			L"██     ",
			L" ██████",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██   ██",
			L"██   ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L"███████",
			L"██     ",
			L"█████  ",
			L"██     ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"███████",
			L"██     ",
			L"█████  ",
			L"██     ",
			L"██     ",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L"██      ",
			L"██   ███",
			L"██    ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"██   ██",
			L"██   ██",
			L"███████",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"████",
			L" ██ ",
			L" ██ ",
			L" ██ ",
			L"████",
			L"    ",
			L"    ",
		},
		{
			L"     ██",
			L"     ██",
			L"     ██",
			L"██   ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L"██   ██",
			L"██  ██ ",
			L"█████  ",
			L"██  ██ ",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██     ",
			L"██     ",
			L"██     ",
			L"██     ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"███    ███",
			L"████  ████",
			L"██ ████ ██",
			L"██  ██  ██",
			L"██      ██",
			L"          ",
			L"          ",
		},
		{
			L"███    ██",
			L"████   ██",
			L"██ ██  ██",
			L"██  ██ ██",
			L"██   ████",
			L"         ",
			L"         ",
		},
		{
			L" ██████ ",
			L"██    ██",
			L"██    ██",
			L"██    ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"██     ",
			L"██     ",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L"██    ██",
			L"██    ██",
			L"██ ▄▄ ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"▄██████",
			L"██     ",
			L"▀█████▄",
			L"     ██",
			L"██████▀",
			L"       ",
			L"       ",
		},
		{
			L"████████",
			L"   ██   ",
			L"   ██   ",
			L"   ██   ",
			L"   ██   ",
			L"        ",
			L"        ",
		},
		{
			L"██    ██",
			L"██    ██",
			L"██    ██",
			L"██    ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"██    ██",
			L"██    ██",
			L"██    ██",
			L" ██  ██ ",
			L"  ████  ",
			L"        ",
			L"        ",
		},
		{
			L"██     ██",
			L"██     ██",
			L"██  █  ██",
			L"██ ███ ██",
			L" ███ ███ ",
			L"         ",
			L"         ",
		},
		{
			L"██   ██",
			L" ██ ██ ",
			L"  ███  ",
			L" ██ ██ ",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██    ██",
			L" ██  ██ ",
			L"  ████  ",
			L"   ██   ",
			L"   ██   ",
			L"        ",
			L"        ",
		},
		{
			L"███████",
			L"   ███ ",
			L"  ███  ",
			L" ███   ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"███",
			L"██ ",
			L"██ ",
			L"██ ",
			L"███",
			L"   ",
			L"   ",
		},
		{
			L"██    ",
			L" ██   ",
			L"  ██  ",
			L"   ██ ",
			L"    ██",
			L"      ",
			L"      ",
		},
		{
			L"███",
			L" ██",
			L" ██",
			L" ██",
			L"███",
			L"   ",
			L"   ",
		},
		{
			L" ▄█▄ ",
			L"█▀ ▀█",
			L"     ",
			L"     ",
			L"     ",
			L"     ",
			L"     ",
		},
		{
			L"       ",
			L"       ",
			L"       ",
			L"       ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"█▄ ",
			L" ▀▀",
			L"   ",
			L"   ",
			L"   ",
			L"   ",
			L"   ",
		},
		{
			L"       ",
			L" ▄▄▄▄  ",
			L"▀   ██ ",
			L"▄█▀▀██ ",
			L"▀█▄▄██▄",
			L"       ",
			L"       ",
		},
		{
			L"██    ",
			L"██▄▄▄ ",
			L"██▀▀██",
			L"██  ██",
			L"█████▀",
			L"      ",
			L"      ",
		},
		{
			L"      ",
			L" ▄▄▄▄▄",
			L"██▀▀▀▀",
			L"██    ",
			L"▀█████",
			L"      ",
			L"      ",
		},
		{
			L"    ██",
			L" ▄▄▄██",
			L"██▀▀██",
			L"██  ██",
			L"▀█████",
			L"      ",
			L"      ",
		},
		{
			L"       ",
			L" ▄▄▄▄▄ ",
			L"██   ██",
			L"██▀▀▀▀ ",
			L"▀█▄▄▄▄ ",
			L"       ",
			L"       ",
		},
		{
			L"  ▄███▄",
			L"  ██   ",
			L"▀█████▀",
			L"  ██   ",
			L"  ██   ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L" ▄▄▄▄ ▄",
			L"██▀▀▀██",
			L"██   ██",
			L"▀██████",
			L"▄▄▄▄▄██",
			L" ▀▀▀▀▀ ",
		},
		{
			L"██    ",
			L"██▄▄▄ ",
			L"██▀▀██",
			L"██  ██",
			L"██  ██",
			L"      ",
			L"      ",
		},
		{
			L"██",
			L"▄▄",
			L"██",
			L"██",
			L"██",
			L"  ",
			L"  ",
		},
		{
			L"  ██",
			L"  ▄▄",
			L"  ██",
			L"  ██",
			L"  ██",
			L"▄▄██",
			L" ▀▀ ",
		},
		{
			L"██     ",
			L"██   ▄▄",
			L"██ ▄█▀ ",
			L"██▀█▄  ",
			L"██  ▀█▄",
			L"       ",
			L"       ",
		},
		{
			L"██  ",
			L"██  ",
			L"██  ",
			L"██  ",
			L"▀██▄",
			L"    ",
			L"    ",
		},
		{
			L"          ",
			L"▄▄▄▄  ▄▄▄ ",
			L"██▀▀██▀▀██",
			L"██  ██  ██",
			L"██  ██  ██",
			L"          ",
			L"          ",
		},
		{
			L"      ",
			L"▄▄▄▄▄ ",
			L"██▀▀██",
			L"██  ██",
			L"██  ██",
			L"      ",
			L"      ",
		},
		{
			L"       ",
			L" ▄▄▄▄▄ ",
			L"██▀▀▀██",
			L"██   ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄ ▄▄▄▄ ",
			L"██▀▀▀██",
			L"██   ██",
			L"█████▀ ",
			L"██     ",
			L"▀▀     ",
		},
		{
			L"       ",
			L" ▄▄▄▄ ▄",
			L"██▀▀▀██",
			L"██   ██",
			L" ▀█████",
			L"     ██",
			L"     ▀▀",
		},
		{
			L"       ",
			L"▄ ▄▄▄▄ ",
			L"███▀▀██",
			L"██     ",
			L"██     ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L" ▄▄▄▄▄ ",
			L"██   ▀▀",
			L" ▀▀▀▀█▄",
			L"▀█▄▄▄█▀",
			L"       ",
			L"       ",
		},
		{
			L"  ██   ",
			L"▄▄██▄▄ ",
			L"▀▀██▀▀ ",
			L"  ██   ",
			L"  ▀███▀",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄▄   ▄▄",
			L"██   ██",
			L"██   ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄▄   ▄▄",
			L"▀█▄ ▄█▀",
			L" ▀█▄█▀ ",
			L"  ▀█▀  ",
			L"       ",
			L"       ",
		},
		{
			L"            ",
			L"▄▄   ▄▄   ▄▄",
			L"▀█▄ ▄██▄ ▄█▀",
			L" ▀█▄█▀▀█▄█▀ ",
			L"  ▀█▀  ▀█▀  ",
			L"            ",
			L"            ",
		},
		{
			L"        ",
			L"▄▄    ▄▄",
			L" ▀█▄▄█▀ ",
			L"  ▄██▄  ",
			L"▄█▀  ▀█▄",
			L"        ",
			L"        ",
		},
		{
			L"       ",
			L"▄     ▄",
			L"▀█▄ ▄█▀",
			L"  ▀██▀ ",
			L"  ▄█   ",
			L" █▀    ",
			L"       ",
		},
		{
			L"       ",
			L"▄▄▄▄▄▄▄",
			L"   ▄█▀ ",
			L" ▄█▀   ",
			L"██▄▄▄▄▄",
			L"       ",
			L"       ",
		},
		{
			L" ▄██",
			L" ██ ",
			L"██  ",
			L" ██ ",
			L" ▀██",
			L"    ",
			L"    ",
		},
		{
			L"██",
			L"██",
			L"██",
			L"██",
			L"██",
			L"  ",
			L"  ",
		},
		{
			L"██▄ ",
			L" ██ ",
			L"  ██",
			L" ██ ",
			L"██▀ ",
			L"    ",
			L"    ",
		},
		{
			L"          ",
			L"  ▄▄      ",
			L"▄█▀▀██▄▄█▀",
			L"      ▀▀  ",
			L"          ",
			L"          ",
			L"          ",
		},
	};


	const wchar_t BIG_RUS_START = L'А';

	const big_char BIG_RUS[] = {
		{
			L" █████ ",
			L"██   ██",
			L"███████",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██     ",
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██     ",
			L"██     ",
			L"██     ",
			L"██     ",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L" ██  ██ ",
			L" ██  ██ ",
			L"████████",
			L"██    ██",
			L"        ",
			L"        ",
		},
		{
			L"███████",
			L"██     ",
			L"█████  ",
			L"██     ",
			L"███████",
			L"       ",
			L"       ",
		},
		{
			L"██  █  ██",
			L" ██ █ ██ ",
			L"  █████  ",
			L" ██ █ ██ ",
			L"██  █  ██",
			L"         ",
			L"         ",
		},
		{
			L"▄█████▄",
			L"     ██",
			L" █████ ",
			L"     ██",
			L"▀█████▀",
			L"       ",
			L"       ",
		},
		{
			L"██    ███",
			L"██   ████",
			L"██  ██ ██",
			L"██ ██  ██",
			L"████   ██",
			L"         ",
			L"         ",
		},
		{
			L"██ ▀▀ ███",
			L"██   ████",
			L"██  ██ ██",
			L"██ ██  ██",
			L"████   ██",
			L"         ",
			L"         ",
		},
		{
			L"██   ██",
			L"██  ██ ",
			L"█████  ",
			L"██  ██ ",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"  ██████",
			L"  ██  ██",
			L"  ██  ██",
			L" ██   ██",
			L"██    ██",
			L"        ",
			L"        ",
		},
		{
			L"███    ███",
			L"████  ████",
			L"██ ████ ██",
			L"██  ██  ██",
			L"██      ██",
			L"          ",
			L"          ",
		},
		{
			L"██   ██",
			L"██   ██",
			L"███████",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L" ██████ ",
			L"██    ██",
			L"██    ██",
			L"██    ██",
			L" ██████ ",
			L"        ",
			L"        ",
		},
		{
			L"███████",
			L"██   ██",
			L"██   ██",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"██     ",
			L"██     ",
			L"       ",
			L"       ",
		},
		{
			L" ██████",
			L"██     ",
			L"██     ",
			L"██     ",
			L" ██████",
			L"       ",
			L"       ",
		},
		{
			L"████████",
			L"   ██   ",
			L"   ██   ",
			L"   ██   ",
			L"   ██   ",
			L"        ",
			L"        ",
		},
		{
			L"██    ██",
			L" ██  ██ ",
			L"  ████  ",
			L"   ██   ",
			L"  ██    ",
			L"        ",
			L"        ",
		},
		{
			L" ▄▄▄█▄▄▄ ",
			L"██▀▀█▀▀██",
			L"██  █  ██",
			L"██▄▄█▄▄██",
			L" ▀▀▀█▀▀▀ ",
			L"         ",
			L"         ",
		},
		{
			L"██   ██",
			L" ██ ██ ",
			L"  ███  ",
			L" ██ ██ ",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"██   ██ ",
			L"██   ██ ",
			L"██   ██ ",
			L"██▄▄▄██ ",
			L" ▀▀▀▀▀██",
			L"        ",
			L"        ",
		},
		{
			L"██   ██",
			L"██   ██",
			L"▀██████",
			L"     ██",
			L"     ██",
			L"       ",
			L"       ",
		},
		{
			L"██  ██  ██",
			L"██  ██  ██",
			L"██  ██  ██",
			L"██▄▄██▄▄██",
			L" ▀▀▀▀▀▀▀▀ ",
			L"          ",
			L"          ",
		},
		{
			L"██  ██  ██ ",
			L"██  ██  ██ ",
			L"██  ██  ██ ",
			L"██▄▄██▄▄██ ",
			L" ▀▀▀▀▀▀▀▀██",
			L"           ",
			L"           ",
		},
		{
			L"████     ",
			L"  ██     ",
			L"  ██████ ",
			L"  ██   ██",
			L"  ██████ ",
			L"         ",
			L"         ",
		},
		{
			L"██      ██",
			L"██      ██",
			L"██████  ██",
			L"██   ██ ██",
			L"██████  ██",
			L"          ",
			L"          ",
		},
		{
			L"██     ",
			L"██     ",
			L"██████ ",
			L"██   ██",
			L"██████ ",
			L"       ",
			L"       ",
		},
		{
			L"█████▄ ",
			L"     ██",
			L"  █████",
			L"     ██",
			L"█████▀ ",
			L"       ",
			L"       ",
		},
		{
			L"██  ██████ ",
			L"██ ██    ██",
			L"█████    ██",
			L"██ ██    ██",
			L"██  ██████ ",
			L"           ",
			L"           ",
		},
		{
			L" ██████",
			L"██   ██",
			L" ██████",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L" ▄▄▄▄  ",
			L"▀   ██ ",
			L"▄█▀▀██ ",
			L"▀█▄▄██▄",
			L"       ",
			L"       ",
		},
		{
			L" ▄▄▄▄▄ ",
			L"█▀     ",
			L"██████▄",
			L"██   ██",
			L"▀█████▀",
			L"       ",
			L"       ",
		},
		{
			L"      ",
			L"▄▄▄▄▄ ",
			L"██   █",
			L"██▀▀▀█",
			L"██▄▄▄▀",
			L"      ",
			L"      ",
		},
		{
			L"      ",
			L"▄▄▄▄▄▄",
			L"██▀▀▀▀",
			L"██    ",
			L"██    ",
			L"      ",
			L"      ",
		},
		{
			L"        ",
			L"  ▄▄▄▄  ",
			L" ██▀▀██ ",
			L" ██  ██ ",
			L"██▀▀▀▀██",
			L"▀▀    ▀▀",
			L"        ",
		},
		{
			L"       ",
			L" ▄▄▄▄▄ ",
			L"██   ██",
			L"██▀▀▀▀ ",
			L"▀█▄▄▄▄ ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄  ▄  ▄",
			L"▀█ █ █▀",
			L" █▀█▀█ ",
			L"█▀ █ ▀█",
			L"       ",
			L"       ",
		},
		{
			L"      ",
			L" ▄▄▄▄ ",
			L"▀▀▀▀██",
			L" ▀▀▀█▄",
			L"▀████▀",
			L"      ",
			L"      ",
		},
		{
			L"       ",
			L"▄▄   ▄▄",
			L"██ ▄███",
			L"███▀ ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"  ▄▄▄  ",
			L"▄▄   ▄▄",
			L"██ ▄███",
			L"███▀ ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄▄   ▄▄",
			L"██ ▄█▀ ",
			L"██▀█▄  ",
			L"██  ▀█▄",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"  ▄▄▄▄▄",
			L" ██▀▀██",
			L" ██  ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"         ",
			L"▄▄     ▄▄",
			L"███▄ ▄███",
			L"██ █▄█ ██",
			L"██  ▀  ██",
			L"         ",
			L"         ",
		},
		{
			L"      ",
			L"▄▄  ▄▄",
			L"██▄▄██",
			L"██▀▀██",
			L"██  ██",
			L"      ",
			L"      ",
		},
		{
			L"       ",
			L" ▄▄▄▄▄ ",
			L"██▀▀▀██",
			L"██   ██",
			L" █████ ",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄▄▄▄▄▄▄",
			L"██▀▀▀██",
			L"██   ██",
			L"██   ██",
			L"       ",
			L"       ",
		},
		{
			L"       ",
			L"▄ ▄▄▄▄ ",
			L"██▀▀▀██",
			L"██   ██",
			L"█████▀ ",
			L"██     ",
			L"▀▀     ",
		},
		{
			L"      ",
			L" ▄▄▄▄▄",
			L"██▀▀▀▀",
			L"██    ",
			L"▀█████",
			L"      ",
			L"      ",
		},
		{
			L"      ",
			L"▄▄▄▄▄▄",
			L"▀▀██▀▀",
			L"  ██  ",
			L"  ██  ",
			L"      ",
			L"      ",
		},
		{
			L"       ",
			L"▄     ▄",
			L"██▄ ▄██",
			L" ▀███▀ ",
			L" ▄██   ",
			L"▄█▀    ",
			L"       ",
		},
		{
			L"    ▄    ",
			L"  ▄▄█▄▄  ",
			L"▄█▀▀█▀▀█▄",
			L"██  █  ██",
			L" ▀█████▀ ",
			L"    █    ",
			L"         ",
		},
		{
			L"      ",
			L"▄    ▄",
			L"▀█▄▄█▀",
			L" ▄██▄ ",
			L"█▀  ▀█",
			L"      ",
			L"      ",
		},
		{
			L"        ",
			L"▄▄   ▄▄ ",
			L"██   ██ ",
			L"██   ██ ",
			L"▀██████▄",
			L"       ▀",
			L"        ",
		},
		{
			L"       ",
			L"▄▄   ▄▄",
			L"██   ██",
			L"▀██████",
			L"     ██",
			L"       ",
			L"       ",
		},
		{
			L"          ",
			L"▄▄  ▄▄  ▄▄",
			L"██  ██  ██",
			L"██  ██  ██",
			L"▀████████▀",
			L"          ",
			L"          ",
		},
		{
			L"           ",
			L"▄▄  ▄▄  ▄▄ ",
			L"██  ██  ██ ",
			L"██  ██  ██ ",
			L"▀█████████▄",
			L"          ▀",
			L"           ",
		},
		{
			L"        ",
			L"▄▄▄▄    ",
			L"▀▀██▄▄▄ ",
			L"  ██▀▀██",
			L"  ██▄▄█▀",
			L"        ",
			L"        ",
		},
		{
			L"         ",
			L"▄▄     ▄▄",
			L"██▄▄▄  ██",
			L"██▀▀██ ██",
			L"██▄▄█▀ ██",
			L"         ",
			L"         ",
		},
		{
			L"      ",
			L"▄▄    ",
			L"██▄▄▄ ",
			L"██▀▀██",
			L"██▄▄█▀",
			L"      ",
			L"      ",
		},
		{
			L"      ",
			L" ▄▄▄▄ ",
			L"▀▀▀▀██",
			L" ▀▀▀██",
			L"▀████▀",
			L"      ",
			L"      ",
		},
		{
			L"          ",
			L"▄▄   ▄▄▄▄ ",
			L"██▄▄██▀▀██",
			L"██▀▀██  ██",
			L"██  ▀████▀",
			L"          ",
			L"          ",
		},
		{
			L"      ",
			L" ▄▄▄▄▄",
			L"██  ██",
			L"▄█▀▀██",
			L"██  ██",
			L"      ",
			L"      ",
			},
	};


	const big_char RUS_YO_UPPER = {
		L" ▀  ▀ ",
		L"██████",
		L"██▄▄▄ ",
		L"██▀▀▀ ",
		L"██████",
		L"      ",
		L"      ",
	};

	const big_char RUS_YO_LOWER = {
		L"  ▄  ▄ ",
		L" ▄▄▄▄▄ ",
		L"██   ██",
		L"██▀▀▀▀ ",
		L"▀█▄▄▄▄ ",
		L"       ",
		L"       ",
	};


	const wchar_t GAP[] = L"  ";
	const int GAP_WIDTH = size(GAP) - 1;

	const int PADDING_X = GAP_WIDTH,
			  PADDING_Y = 2;



	#ifndef NDEBUG
	static string getErrorMessage(int i, int j, const char* description) {
		return i == -1 ?
				"Line #" + to_string(j) + ' ' + description :
				"Line #" + to_string(i) + " #" + to_string(j) + ' ' + description;
	}

	static bool checkBigChar(const big_char& chr, int i = -1) {
		size_t len = -1;

		for (int j = 0; j < CHAR_HEIGHT; ++j) {
			const wchar_t* line = chr[j];

			if (line == nullptr) {
				throw logic_error(getErrorMessage(i, j, "is null"));
			}

			if (j == 0) {
				len = wcslen(line);
			} else {
				if (len != wcslen(line)) {
					throw logic_error(getErrorMessage(i, j, "has different size"));
				}
			}
		}

		return true;
	}

	template<int N>
	static bool checkBigCharArray(const big_char(& arr)[N]) {
		for (int i = 0; i < N; ++i) {
			checkBigChar(arr[i], i);
		}

		return true;
	}

	static const bool checked =
			checkBigChar(UNKNOWN_CHAR) &&
			checkBigChar(EMPTY_CHAR) &&
			checkBigCharArray(BIG_ASCII) &&
			checkBigCharArray(BIG_RUS) &&
			checkBigChar(RUS_YO_UPPER) &&
			checkBigChar(RUS_YO_LOWER);

	#endif /* not NDEBUG */


	static const big_char& getBigChar(wchar_t ch) {
		switch (ch) {
			case '\0': case '\n':
				return EMPTY_CHAR;
			
			case '\t': case '\v': case '\f': case '\r':
				return BIG_ASCII[0];
			
			case L'Ё': return RUS_YO_UPPER;
			case L'ё': return RUS_YO_LOWER;
			
			default:
				if (ch >= BIG_ASCII_START && ch <= static_cast<wchar_t>(BIG_ASCII_START + size(BIG_ASCII))) {
					return BIG_ASCII[ch - BIG_ASCII_START];
				}

				if (ch >= BIG_RUS_START && ch <= static_cast<wchar_t>(BIG_RUS_START + size(BIG_ASCII))) {
					return BIG_RUS[ch - BIG_RUS_START];
				}

				return UNKNOWN_CHAR;
		}
	}

	static int bigCharWidth(const big_char& chr) {
		return static_cast<int>(wcslen(chr[0])) + GAP_WIDTH;
	}


	static void drawBorder(int sx, int sy, int ex, int ey) {
		mvaddch(sy, sx, ACS_ULCORNER);
		mvaddch(sy, ex, ACS_URCORNER);
		mvaddch(ey, sx, ACS_LLCORNER);
		mvaddch(ey, ex, ACS_LRCORNER);

		for (int x = sx + 1; x < ex; ++x) {
			mvaddch(sy, x, ACS_HLINE);
			mvaddch(ey, x, ACS_HLINE);
		}

		for (int y = sy + 1; y < ey; ++y) {
			mvaddch(y, sx, ACS_VLINE);
			mvaddch(y, ex, ACS_VLINE);
		}
	}


	static void drawPadding(int sx, int sy, int ex, int ey) {
		for (int i = 0; i < PADDING_Y; ++i) {
			move(sy + i, sx);

			for (int x = sx; x < ex; ++x) {
				addch(' ');
			}
		}

		for (int i = 0; i < PADDING_X; ++i) {
			for (int y = sy; y < ey; ++y) {
				mvaddch(y, sx + i, ' ');
			}
		}
	}

	static void drawText(const wstring& message, const vector<pair<int, int>>& breaks,
			bool small, int lineHeight, int tx, int ty, int ex) {
		
		int y = ty;
		for (const auto& entry : breaks) {
			int l = entry.second - entry.first + 1;

			if (small) {
				mvaddnwstr(y, tx, message.c_str() + entry.first, l);

				for (l += tx; l < ex; ++l) {
					addch(' ');
				}

			} else {
				const wchar_t
						*wstr = message.c_str() + entry.first,
						*end = wstr + l;

				int x = tx;

				for (; wstr < end; wstr++) {
					const big_char& chr = getBigChar(*wstr);
					int chrWidth = bigCharWidth(chr);

					for (int i = 0; i < CHAR_HEIGHT; ++i) {
						mvaddwstr(y + i, x, chr[i]);
						addwstr(GAP);
					}

					x += chrWidth;
				}

				for (int i = 0; i < CHAR_HEIGHT; ++i) {
					move(y + i, x);

					for (int j = x; j < ex; ++j) {
						addch(' ');
					}
				}
			}

			y += lineHeight;
		}
	}


	static wstring buffer;
	static const wstring* alerted = nullptr;

	std::tuple<int, int, int, int> alert(const std::wstring& message) {
		const bool small = !bigCharsMode;

		const int lineHeight = small ? 1 : CHAR_HEIGHT;

		const int offsetX = small ? 0 : PADDING_X,
				  offsetY = small ? 0 : PADDING_Y;


		int width = 0,
			height = 0;

		int lineWidth = 0,
			maxLineWidth = COLS - 2 - offsetX;
		
		vector<pair<int, int>> breaks;

		for (size_t p = 0, i = 0, size = message.size(); i <= size; ++i) {
			wchar_t ch = message[i];

			bool isEol = ch == L'\n' || ch == L'\0';

			int newLineWidth = lineWidth + (isEol ? 0 : small ? 1 : bigCharWidth(getBigChar(ch)));

			if (newLineWidth <= maxLineWidth) {
				lineWidth = newLineWidth;
			} else {
				--i;
			}

			if (isEol || newLineWidth >= maxLineWidth) {
				width = max(width, lineWidth);
				
				lineWidth = 0;
				height += lineHeight;

				size_t np = isEol ? i - 1 : i;
				breaks.emplace_back(p, np);
				p = i + 1;
			}
		}

		width += offsetX;
		height += offsetY;


		int tx = max((COLS - width) / 2, offsetX + 1),
			ty = max((LINES - height) / 2, offsetY + 1);

		int sx = tx - 1 - offsetX,
			sy = ty - 1 - offsetY,
			ex = sx + 1 + width,
			ey = sy + 1 + height;


		drawText(message, breaks, small, lineHeight, tx, ty, ex);
		
		if (!small) {
			drawPadding(sx + 1, sy + 1, ex, ey);
		}
		
		drawBorder(sx, sy, ex, ey);

		buffer = message;
		alerted = &buffer;

		return {sx, sy, ex + 1, ey + 1};
	}


	std::tuple<int, int, int, int> realert() {
		if (alerted != nullptr) {
			return alert(*alerted);
		}

		return {0, 0, 0, 0};
	}

	void resetAlerted() {
		alerted = nullptr;
		buffer.clear();
	}
}
