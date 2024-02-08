#ifndef NCV_IMAGE_CPP
#define NCV_IMAGE_CPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#pragma GCC diagnostic pop

#include "rgb.cpp"
#include "ncv_assert.h"

namespace ncv {

	const int BPP = 4;

	const int // Для alpha канала
			BACKGROUND_R = 0x3F,
			BACKGROUND_G = 0x3F,
			BACKGROUND_B = 0x3F;

	class Image {
		uint8_t* data;
		int width, height, bpp;

	public:
		Image(): data(nullptr), width(0), height(0), bpp(0) {}

		Image(const char* name) {
			load(name);
			removeAlpha();
		}
		
		~Image() {
			if (data != nullptr) {
				stbi_image_free(data);
				data = nullptr;
			}
		}

		void load(const char* name) {
			this->~Image();

			data = stbi_load(name, &width, &height, &bpp, BPP);
		}

		void copyFrom(const Image& other, int newWidth, int newHeight) {
			this->~Image();

			data = resize(other.data, other.width, other.height, newWidth, newHeight);
			width = newWidth;
			height = newHeight;
			bpp = other.bpp;
		}
		
		bool success() const {
			return data != NULL;
		}

		int getWidth() const {
			return width;
		}

		int getHeight() const {
			return height;
		}

	protected:
		uint8_t* getData() const {
			return data;
		}


		void removeAlpha() {
			if (bpp == 4) { // alpha channel
				for (int i = 0, s = width * height * 4; i < s; i += 4) {
					int a = data[i+3],
						r = data[i+0],
						g = data[i+1],
						b = data[i+2];
					
					data[i+0] = BACKGROUND_R + (r - BACKGROUND_R) * a / 255;
					data[i+1] = BACKGROUND_G + (g - BACKGROUND_G) * a / 255;
					data[i+2] = BACKGROUND_B + (b - BACKGROUND_B) * a / 255;
				}

				bpp = 3;
			}
		}


		static uint8_t* resize(uint8_t* data, int width, int height, int newWidth, int newHeight) {
			uint8_t* newData = new uint8_t[newWidth * newHeight * BPP];

			stbir_resize(
				data, width, height, 0, newData, newWidth, newHeight, 0,
				STBIR_TYPE_UINT8, BPP, STBIR_ALPHA_CHANNEL_NONE, 0,
				STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
				STBIR_FILTER_BOX, STBIR_FILTER_BOX,
				STBIR_COLORSPACE_SRGB, NULL
			);

			return newData;
		}

	public:
		void resize(int newWidth, int newHeight) {
			if (newWidth == width && newHeight == height) {
				return;
			}

			uint8_t* newData = resize(data, width, height, newWidth, newHeight);

			stbi_image_free(data);
			data = newData;
			width = newWidth;
			height = newHeight;
		}

		rgb_t pixel(int index) const {
			ASSERT(index < width * height);
			index *= BPP;
			return getColor(data[index], data[index + 1], data[index + 2]);
		}

		rgb_t pixel(int x, int y) const {
			return pixel(y * width + x);
		}

		class iterator: std::iterator<std::forward_iterator_tag, rgb_t> {
			friend class Image;

			uint8_t *data;
		
			iterator(uint8_t* data): data(data) {}
			
		public:
			iterator(const iterator& other): data(other.data) {}

			bool operator==(iterator const& other) const {
				return data == other.data;
			}
			
			bool operator!=(iterator const& other) const {
				return data != other.data;
			}

			value_type operator*() const {
				return getColor(data[0], data[1], data[2]);
			}

			iterator& operator++() {
				data += BPP;
				return *this;
			}
		};

		typedef iterator const_iterator;

		const_iterator begin() const {
			return iterator(data);
		}

		const_iterator end() const {
			return iterator(data + width * height * BPP);
		}
	};
}

#endif /* NCV_IMAGE_CPP */