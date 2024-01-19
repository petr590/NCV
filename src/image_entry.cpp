#ifndef NCV_IMAGE_ENTRY_CPP
#define NCV_IMAGE_ENTRY_CPP

#include "quantizied_image.cpp"

namespace ncv {

	class ImageEntry {
		Image src;
		QuantiziedImage processed;
		bool noise = false;
		bool needReprocess = false;

	public:
		bool empty() const {
			return !src.success();
		}

		bool success() const {
			return src.success();
		}
		
		void load(const char* name) {
			src.load(name);
		}

		void toggleNoise() {
			noise = !noise;
			needReprocess = true;
		}

		void process(int scrWidth, int scrHeight, int maxColors) {
			int imgWidth = src.getWidth(),
				imgHeight = src.getHeight();
			
			double ratio = min(min(static_cast<double>(scrWidth) / imgWidth, static_cast<double>(scrHeight) / imgHeight), 1.0);
			
			int newWidth = static_cast<int>(round(imgWidth * ratio)),
				newHeight = static_cast<int>(round(imgHeight * ratio));
			
			if (newWidth == processed.getWidth() && newHeight == processed.getHeight() && !needReprocess) {
				return;
			}

			processed.copyFrom(src, newWidth, newHeight);

			if (noise) {
				processed.addNoise();
			}

			processed.quantise(maxColors);
			
			assert(processed.getColorTable().size() <= static_cast<size_t>(maxColors));

			needReprocess = false;
		}

		const vector<int>& getColorTable() const {
			return processed.getColorTable();
		}

		void draw() const {
			processed.draw();
		}
	};
}

#endif /* NCV_IMAGE_ENTRY_CPP */