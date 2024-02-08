#ifndef NCV_IMAGE_ENTRY_CPP
#define NCV_IMAGE_ENTRY_CPP

#include "quantizied_image.cpp"
#include <set>

namespace ncv {
	using std::set;

	class ImageEntry {
		Image src;
		QuantiziedImage processed;
		bool noise = false;
		bool needReprocess = false;

		bool hasContrastColor = false;
		bool foregroundColorInited = false;
		int reserved = 0;

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

		void process(int scrWidth, int scrHeight, int maxColors, rgb_t background) {
			int imgWidth = src.getWidth(),
				imgHeight = src.getHeight();
			
			double ratio = min(min(static_cast<double>(scrWidth) / imgWidth, static_cast<double>(scrHeight) / imgHeight), 1.0);
			
			int newWidth = static_cast<int>(round(imgWidth * ratio)),
				newHeight = static_cast<int>(round(imgHeight * ratio));
			
			if (newWidth == processed.getWidth() && newHeight == processed.getHeight() && !needReprocess) {
				return;
			}


			// Резервируем индексы для фона и текста
			hasContrastColor = hasContrastColorFor(background);
			reserved = hasContrastColor ? 1 : 2;
			foregroundColorInited = false;


			// Сама обработка изображения
			processed.copyFrom(src, newWidth, newHeight);

			if (noise)
				processed.addNoise();

			processed.quantize(maxColors - reserved, !hasContrastColor);

			size_t colorTableSize = processed.getColorTable().size();
			
			ASSERT_MESSAGE(
				colorTableSize <= static_cast<size_t>(maxColors),
				"Color table size = %llu, maxColors = %d", colorTableSize, maxColors
			);

			needReprocess = false;
		}

		inline const vector<rgb_t>& getColorTable() const {
			return processed.getColorTable();
		}

		inline bool quantized() const {
			return processed.quantized();
		}

		inline void draw() const {
			processed.draw();
		}

		inline void draw(int sx, int sy, int ex, int ey) const {
			processed.drawInChars(sx, sy, ex, ey);
		}


		void initForegroundColor(rgb_t background) {
			if (foregroundColorInited)
				return;

			if (hasContrastColor && getColorTable().size() >= FOREGROUND_INDEX) {
				processed.swap(mostContrastColorIndex(background), FOREGROUND_INDEX);

			} else {
				init_color(FOREGROUND_INDEX, 1000, 1000, 1000);
			}

			foregroundColorInited = true;
		}


		void initColors(rgb_t background) {
			initForegroundColor(background);
			processed.initColors();
		}


	private:
		bool hasContrastColorFor(rgb_t color) const {
			float bright = brightness(color);

			const Image& src = this->src;

			for (auto i = src.begin(); i != src.end(); ++i) {
				if (abs(brightness(*i) - bright) >= BREIGHTNESS_TRESHOLD) {
					return true;
				}
			}

			return false;
		}

		int mostContrastColorIndex(rgb_t color) const {
			const vector<rgb_t>& colorTable = getColorTable();

			float bright = brightness(color);
			float maxDiff = 0;
			int res = 0;

			for (int i = 0, size = colorTable.size(); i < size; ++i) {
				float diff = abs(brightness(colorTable[i]) - bright);

				if (diff > maxDiff) {
					maxDiff = diff;
					res = i;
				}
			}

			return res;
		}
	};
}

#endif /* NCV_IMAGE_ENTRY_CPP */