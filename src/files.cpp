#ifndef NCV_FILES_CPP
#define NCV_FILES_CPP

#include <string>
#include <filesystem>
#include <algorithm>

namespace ncv {
	namespace fs = std::filesystem;
	using std::string;
	using std::wstring;
	using std::pair;
	using std::vector;

	class File {
		fs::path fPath;
		wstring fWpath;
		size_t fSize;

	public:
		File(const fs::path& path):
				fPath(path), fSize(fs::file_size(path)) {
			
			// path.wstring() падает с ошибкой, поэтому так

			const string& str = path.string();

			fWpath = wstring(str.size(), L' ');
			fWpath.resize(std::mbstowcs(&fWpath[0], str.c_str(), str.size()));
		}
		
		const fs::path& path() const {
			return fPath;
		}

		const wstring& wpath() const {
			return fWpath;
		}
		
		size_t size() const {
			return fSize;
		}
		
		bool operator<(const File& other) const {
			return fPath < other.fPath;
		}
	};


	pair<vector<File>, int> findFiles(fs::path fileOrDir) {
		vector<File> files;
		int index = 0;

		fs::path firstFile;

		if (!fs::is_directory(fileOrDir)) {
			firstFile = fileOrDir.filename();
		
			fileOrDir = fileOrDir.parent_path();
			
			if (fileOrDir.empty())
				fileOrDir = ".";
		}


		const vector<string> extensions { ".png", ".jpg", ".gif", ".bmp", ".psd", ".pic", ".pnm" };
		bool firstFileFound = false;


		for (const auto& entry : fs::directory_iterator(fileOrDir)) {
			const fs::path& path = entry.path();
			
			std::error_code ec; // Ignore errors

			if (fs::is_regular_file(path, ec)) {
				if (!firstFile.empty() && !firstFileFound && path.filename() == firstFile) {
					files.push_back(File(path));
					firstFileFound = true;
					continue;
				}

				if (find(extensions.begin(), extensions.end(), path.extension()) != extensions.end()) {
					files.push_back(File(path));
				}
			}
		}

		sort(files.begin(), files.end());

		if (firstFileFound) {
			for (int i = 0, size = files.size(); i < size; ++i) {
				if (files[i].path().filename() == firstFile) {
					index = i;
					break;
				}
			}
		}

		return {files, index};
	}
}

#endif /* NCV_FILES_CPP */