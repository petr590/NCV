#ifndef NCV_FILES_H
#define NCV_FILES_H

#include <string>
#include <vector>
#include <filesystem>
#include <ncurses.h>

namespace ncv {
	class File {
		std::filesystem::path fPath;
		std::wstring fWpath;

		size_t size;
		float normalizedSize;
		const char* sizeUnit;

	public:
		explicit File(const std::filesystem::path& path);
		
		const std::filesystem::path& path() const;

		const std::wstring& wpath() const;
		
		void printToWindow(WINDOW*) const;
		
		bool operator<(const File& other) const;

		friend std::ostream& operator<<(std::ostream& out, const File& file) {
			return out << file.fPath;
		}
	};


	extern std::pair<std::vector<File>, size_t> findFiles(std::filesystem::path fileOrDir);
}

#endif /* NCV_FILES_H */