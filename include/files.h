#ifndef NCV_FILES_H
#define NCV_FILES_H

#include <string>
#include <vector>
#include <filesystem>

namespace ncv {
	namespace fs = std::filesystem;

	class File {
		fs::path fPath;
		std::wstring fWpath;

		size_t fSize;
		float fNormalizedSize;
		const char* fSizeUnit;

	public:
		File(const fs::path& path);
		
		const fs::path& path() const;

		const std::wstring& wpath() const;
		
		size_t size() const;

		float normalizedSize() const;

		const char* sizeUnit() const;
		
		bool operator<(const File& other) const;

		friend std::ostream& operator<<(std::ostream& out, const File& file) {
			return out << file.fPath;
		}
	};


	extern std::pair<std::vector<File>, size_t> findFiles(fs::path fileOrDir);
}

#endif /* NCV_FILES_H */