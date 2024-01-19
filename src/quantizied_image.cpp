#ifndef NCV_QUANTIZIED_IMAGE_CPP
#define NCV_QUANTIZIED_IMAGE_CPP

#include <ncurses.h>

#include "image.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <limits>
#include "dynamic_bitset.cpp"


namespace ncv {
	using std::vector;
	using std::map;
	using std::pair;

	using std::min;
	using std::max;

	using std::cout;
	using std::cerr;
	using std::endl;
	using std::hex;
	using std::dec;

	using std::numeric_limits;

	const float
			RM = 0.299f,
			GM = 0.587f,
			BM = 0.114f;
	
	const int NOISE_AMPLITUDE = 3;
	

	class QuantiziedImage: public Image {
		map<int, int> indexTable;
		vector<int> colorTable;

	public:
		QuantiziedImage(): Image() {}

		QuantiziedImage(const char* name): Image(name) {}


		void addNoise() {
			uint8_t* data = getData();

			for (int i = 0, s = getWidth() * getHeight() * BPP; i < s; i += BPP) {
				data[i+0] = max(min(data[i+0] + (rand() % (NOISE_AMPLITUDE * 2 + 1) - NOISE_AMPLITUDE), 0xFF), 0);
			}
		}

	
	private:
		struct block {
			uint64_t r = 0, g = 0, b = 0, count = 0;
			int index = -1;
			
			block() {}

			inline bool inited() const {
				return index != -1;
			}
			
			int32_t avg() const {
				auto c = count;
				return c == 0 ? -1 : getColor(r / c, g / c, b / c);
			}

			int getIndex(int* last) {
				return index != -1 ? index : (index = (*last)++);
			}
		};


		struct context {
		private:
			const map<int, int>& pixelMap;
			const uint32_t colors;

			int minR = 0xFF, minG = 0xFF, minB = 0xFF;
			int   cntR = 1, cntG = 1, cntB = 1;
			float lenR = 0, lenG = 0, lenB = 0;

		public:
			context(const map<int, int>& pixelMap, uint32_t colors):
				pixelMap(pixelMap), colors(colors) {}


			void initLen() {
				int maxR = 0,
					maxG = 0,
					maxB = 0;

				for (const auto& pixel : pixelMap) {
					int color = pixel.first;
					minR = min(minR, getR(color));
					minG = min(minG, getG(color));
					minB = min(minB, getB(color));
					maxR = max(maxR, getR(color));
					maxG = max(maxG, getG(color));
					maxB = max(maxB, getB(color));
				}
				
				lenR = maxR - minR;
				lenG = maxG - minG;
				lenB = maxB - minB;
			}


			void medianCross() {
				for (uint32_t parts = 1; parts < colors || hasPlace(); parts *= 2) {
					float maxLen = max(lenR * RM, max(lenG * GM, lenB * BM));
					
					if (maxLen == lenR * RM)
						lenR /= 2, cntR *= 2;
					else if (maxLen == lenG * GM)
						lenG /= 2, cntG *= 2;
					else
						lenB /= 2, cntB *= 2;
				}
			}


			bool hasPlace() {
				dynamic_bitset bitset(cntR * cntG * cntB);

				for (const auto& pixel : pixelMap) {
					int color = pixel.first;
						
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					bitset[(ri * cntG + gi) * cntB + bi] = true;
				}

				return bitset.count() * 2 <= colors;
			}



			void calcTables(map<int, int>& indexTable, vector<int>& colorTable) const {
				block data[cntR][cntG][cntB] = {};
			
				for (const auto& pixel : pixelMap) {
					int color = pixel.first;
					
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					assert(ri < cntR);
					assert(gi < cntG);
					assert(bi < cntB);
					
					block& blk = data[ri][gi][bi];
					
					blk.r += getR(color) * pixel.second;
					blk.g += getG(color) * pixel.second;
					blk.b += getB(color) * pixel.second;
					blk.count += pixel.second;
				}
				

				int lastIndex = 0;

				for (const auto& pixel : pixelMap) {
					int color = pixel.first;
					
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					assert(ri < cntR);
					assert(gi < cntG);
					assert(bi < cntB);
					
					indexTable[color] = data[ri][gi][bi].getIndex(&lastIndex);
				}

				colorTable.resize(lastIndex);

				for (int ri = 0; ri < cntR; ++ri) {
					for (int gi = 0; gi < cntG; ++gi) {
						for (int bi = 0; bi < cntB; ++bi) {

							const block& blk = data[ri][gi][bi];

							if (blk.inited()) {
								colorTable[blk.index] = blk.avg();
							}
						}
					}
				}
			}

			
			static inline float zeroIfNaN(float x) {
				return isnan(x) ? 0 : x;
			}


			int getRi(int color) const {
				int i = min(zeroIfNaN((getR(color) - minR) / lenR), cntR - 1.f);
				assert(i >= 0);
				return i;
			}

			int getGi(int color) const {
				int i = min(zeroIfNaN((getG(color) - minG) / lenG), cntG - 1.f);
				assert(i >= 0);
				return i;
			}

			int getBi(int color) const {
				int i = min(zeroIfNaN((getB(color) - minB) / lenB), cntB - 1.f);
				assert(i >= 0);
				return i;
			}
		};


	public:
		void quantise(const uint32_t colors) {
			map<int, int> pixelMap;
			
			for (int i = 0, s = getWidth() * getHeight(); i < s; ++i) {
				int color = pixel(i);
				
				const auto& entry = pixelMap.find(color);
				if (entry == pixelMap.end())
					pixelMap[color] = 1;
				else
					entry->second += 1;
			}

			if (pixelMap.size() <= colors) {
				// use pixelMap as indexTable
				for (auto& pixel : pixelMap) {
					pixel.second = colorTable.size();
					colorTable.push_back(pixel.first);
				}
				
				indexTable = pixelMap;
				return;
			}

			context ctx(pixelMap, colors);
			
			ctx.initLen();
			ctx.medianCross();
			ctx.calcTables(indexTable, colorTable);
		}


		const vector<int>& getColorTable() const {
			return colorTable;
		}


		void draw() const {
			int width = getWidth(),
				height = getHeight();
			
			int sx = (COLS - width * 2) / 2,
				sy = (LINES - height) / 2;

			for (int y = 0; y < height; ++y) {
				int yw = y * width;

				move(sy + y, sx);

				for (int x = 0; x < width; ++x) {
					int ch = ' ' | COLOR_PAIR(indexTable.at(pixel(yw + x)) + 1); // Индекс 0 зарезервирован
					addch(ch);
					addch(ch);
				}
			}
		}
	};
}

#endif /* NCV_QUANTIZIED_IMAGE_CPP */