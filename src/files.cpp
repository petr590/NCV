#include "files.h"
#include <cmath>
#include <algorithm>

namespace ncv {
	using std::string;
	using std::wstring;
	using std::vector;
	using std::pair;

	File::File(const fs::path& path):
			fPath(path), fSize(fs::file_size(path)) {
		
		// path.wstring() падает с ошибкой, поэтому так

		const string& str = path.string();

		fWpath = wstring(str.size(), L' ');
		fWpath.resize(std::mbstowcs(&fWpath[0], str.c_str(), str.size()));


		static const vector<const char*> units = {"B", "KB", "MB", "GB", "TB"};

		float value = this->size();
		size_t i = 0;

		while (value >= 1024 && i < units.size() - 1) {
			value /= 1024;
			i += 1;
		}

		fNormalizedSize = value;
		fSizeUnit = units[i];
	}
	
	const fs::path& File::path() const {
		return fPath;
	}

	const wstring& File::wpath() const {
		return fWpath;
	}
	
	size_t File::size() const {
		return fSize;
	}

	float File::normalizedSize() const {
		return fNormalizedSize;
	}

	const char* File::sizeUnit() const {
		return fSizeUnit;
	}
	
	bool File::operator<(const File& other) const {
		return fPath < other.fPath;
	}


	pair<vector<File>, size_t> findFiles(fs::path fileOrDir) {
		vector<File> files;
		size_t index = 0;

		fs::path firstFile;

		if (!fs::is_directory(fileOrDir)) {
			firstFile = fileOrDir.filename();
		
			fileOrDir = fileOrDir.parent_path();
			
			if (fileOrDir.empty())
				fileOrDir = ".";
		}


		bool firstFileFound = false;


		static const vector<string> extensions = {
			".png", ".jpg", ".jpeg", ".gif", ".bmp", ".psd", ".pic", ".pnm", ".raw", ".tiff",
			".3g2", ".3gp", ".asf", ".avi", ".cine", ".dash", ".dv", ".f4v", ".flic", ".flv",
			".gxf", ".h261", ".h263", ".h264", ".hevc", ".ifv", ".ipod", ".ismv", ".ivf", ".m4a",
			".m4v", ".mj2", ".mjpeg", ".mov", ".mp4", ".mpeg", ".mtv", ".mxf", ".nuv", ".ogv",".r3d", ".rm", ".sol", ".swf", ".vc1", ".vivo", ".vob", ".webm", ".wtv", ".wve"
		};


		for (const auto& entry : fs::directory_iterator(fileOrDir)) {
			const fs::path& path = entry.path();

			if (fs::is_regular_file(path)) {
				if (!firstFile.empty() && !firstFileFound && path.filename() == firstFile) {
					files.emplace_back(path);
					firstFileFound = true;
					continue;
				}

				string ext = path.extension();

				transform(ext.begin(), ext.end(), ext.begin(),
    				[] (unsigned char c) { return tolower(c); });
				
				
				if (find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
					files.emplace_back(path);
				}
			}
		}

		sort(files.begin(), files.end());

		if (firstFileFound) {
			for (size_t i = 0, size = files.size(); i < size; ++i) {
				if (files[i].path().filename() == firstFile) {
					index = i;
					break;
				}
			}
		}

		return {std::move(files), index};
	}
}