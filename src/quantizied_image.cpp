#ifndef NCV_QUANTIZIED_IMAGE_CPP
#define NCV_QUANTIZIED_IMAGE_CPP

#include "image.cpp"
#include "dynamic_bitset.cpp"
#include "int_pair.cpp"
#include <vector>
#include <map>
#include <set>


#ifdef DEBUG

#include <fstream>

namespace ncv {
	std::ofstream log("log.txt");

	using std::endl;
	using std::hex;
	using std::dec;
}

#endif

namespace ncv {
	using std::vector;
	using std::map;
	using std::set;
	using std::pair;

	using std::min;
	using std::max;
	
	const int NOISE_AMPLITUDE = 3;

	// Нулевая цветовая пара всегда использует эти индексы, её нельзя изменить
	const int BACKGROUND_INDEX = COLOR_BLACK,
			  FOREGROUND_INDEX = COLOR_WHITE;
	
	const rgb_t BACKGROUND_COLOR = 0x000000,
				FOREGROUND_COLOR = 0xFFFFFF;

	#define USE_BITSET 1
	

	class QuantiziedImage: public Image {
		map<rgb_t, int> indexTable;
		vector<rgb_t> colorTable;

	public:
		QuantiziedImage(): Image() {}

		QuantiziedImage(const char* name): Image(name) {}

		const vector<rgb_t>& getColorTable() const {
			return colorTable;
		}

		void addNoise() {
			uint8_t* data = getData();

			for (int i = 0, s = getWidth() * getHeight() * BPP; i < s; i += BPP) {
				data[i+0] = max(min(data[i+0] + (rand() % (NOISE_AMPLITUDE * 2 + 1) - NOISE_AMPLITUDE), 0xFF), 0);
				data[i+1] = max(min(data[i+1] + (rand() % (NOISE_AMPLITUDE * 2 + 1) - NOISE_AMPLITUDE), 0xFF), 0);
				data[i+2] = max(min(data[i+2] + (rand() % (NOISE_AMPLITUDE * 2 + 1) - NOISE_AMPLITUDE), 0xFF), 0);
			}
		}


	private:
		struct index_generator {
		private:
			const bool skipForeground;
			int value = BACKGROUND_INDEX + 1;

		public:
			index_generator(bool skipForeground):
					skipForeground(skipForeground) {}
			
			inline bool skips() const {
				return skipForeground && value == FOREGROUND_INDEX - 1;
			}

			inline index_generator& operator++() {
				value += (skips() ? 2 : 1);
				return *this;
			}

			inline index_generator operator++(int) {
				index_generator copy = *this;
				this->operator++();
				return copy;
			}

			inline operator int() const {
				return value;
			}
		};

	
		struct block {
			uint64_t r = 0, g = 0, b = 0, c = 0;

		private:
			int index = -1;
			
			// Блок, объединённый с этим. Работают вместе как один блок
			block* other = nullptr;
			
		public:
			block() {}

			inline bool inited() const {
				return index != -1;
			}
			
			int32_t avg() const {
				auto c = this->c;
				return c == 0 ? -1 : getColor(r / c, g / c, b / c);
			}

			int getIndex() const {
				return index;
			}

			int getIndex(index_generator* last) {
				if (index != -1)
					return index;
				
				int res = index = (*last)++;

				if (other != nullptr)
					other->index = res;

				return res;
			}

			bool joined() const {
				return other != nullptr;
			}

			void join(block* other) {
				ASSERT(!joined() && !other->joined());

				this->other = other;
				other->other = this;

				this->r = other->r = this->r + other->r;
				this->g = other->g = this->g + other->g;
				this->b = other->b = this->b + other->b;
				this->c = other->c = this->c + other->c;
			}
		};


		struct context {
		private:
			const map<rgb_t, int>& pixelMap; // Ключ: цвет, значение: количество в изображении
			const uint32_t colors;
			const bool skipForeground; // Скипать или нет FOREGROUND_INDEX в таблицах

			int minR = 0xFF, minG = 0xFF, minB = 0xFF;
			int   cntR = 1, cntG = 1, cntB = 1;
			float lenR = 0, lenG = 0, lenB = 0;

			uint32_t filledBlocks = -1; // Кэшируем, чтобы не считать ещё раз
			vector<vector<vector<block>>> blocks;

			inline bool maximizeColors() {
				return colors <= 256;
			}

			inline int colorsMultiplier() {
				return maximizeColors() ? 1 : 2;
			}

		public:
			context(const map<rgb_t, int>& pixelMap, uint32_t colors, bool skipForeground):
				pixelMap(pixelMap), colors(colors), skipForeground(skipForeground) {}


			void initLen() {
				int maxR = 0,
					maxG = 0,
					maxB = 0;

				for (const auto& pixel : pixelMap) {
					rgb_t color = pixel.first;
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
				for (uint32_t parts = colorsMultiplier(); parts < colors || hasPlace(); parts *= 2) {
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
				#if USE_BITSET
				dynamic_bitset used(static_cast<size_t>(cntR) * cntG * cntB);
				#else
				map<int, bool> used;
				#endif

				for (const auto& pixel : pixelMap) {
					rgb_t color = pixel.first;
						
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					used[(ri * cntG + gi) * cntB + bi] = true;
				}

				#if USE_BITSET
				filledBlocks = used.count();
				#else
				filledBlocks = used.size();
				#endif

				return filledBlocks * colorsMultiplier() <= colors;
			}


			void __attribute__ ((noinline)) joinBlocks() { // Количество цветов может быть больше, чем colors
				uint32_t filled = filledBlocks;
				
				// Таблица смещений координат
				int offsets[3][3] = {
				  // x y z
					{0,0,1},
					{0,1,0},
					{1,0,0},
				};

				// Ищем пару соседних блоков с минимальным кол-вом цветов и объединяем
				while (filled > colors) {
					uint64_t minColors = 0x7FFFFFFF;
					block *minBlk1 = nullptr,
						  *minBlk2 = nullptr;
					
					for (int ri = 0; ri < cntR; ++ri) {
						for (int gi = 0; gi < cntG; ++gi) {
							for (int bi = 0; bi < cntB; ++bi) {
								block& blk1 = blocks[ri][gi][bi];

								if (blk1.c == 0 || blk1.joined())
									continue;

								uint64_t c = blk1.c;

								for (int* offset : offsets) {
									int ri2 = ri + offset[0],
										gi2 = gi + offset[1],
										bi2 = bi + offset[2];
									
									if (ri2 < cntR && gi2 < cntG && bi2 < cntB) {
										block& blk2 = blocks[ri2][gi2][bi2];

										if (blk2.c > 0 && !blk2.joined() && blk2.c + c < minColors) {
											minColors = blk2.c + c;
											minBlk1 = &blk1;
											minBlk2 = &blk2;
										}
									}
								}
							}
						}
					}

					minBlk1->join(minBlk2);
					filled -= 1;
				}
			}

			
			void calcTables(map<rgb_t, int>& indexTable, vector<rgb_t>& colorTable) {

				blocks = vector<vector<vector<block>>>(cntR, vector<vector<block>>(cntG, vector<block>(cntB)));

				// Заполняем таблицу блоков
				for (const auto& pixel : pixelMap) {
					rgb_t color = pixel.first;
					
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					ASSERT(ri < cntR);
					ASSERT(gi < cntG);
					ASSERT(bi < cntB);
					
					block& blk = blocks[ri][gi][bi];
					
					blk.r += getR(color) * pixel.second;
					blk.g += getG(color) * pixel.second;
					blk.b += getB(color) * pixel.second;
					blk.c += pixel.second;
				}


				if (maximizeColors()) {
					joinBlocks();
				}
				

				// Инициализируем индексы цветов

				indexTable.clear();

				index_generator lastIndex(skipForeground);

				for (const auto& pixel : pixelMap) {
					rgb_t color = pixel.first;
					
					int ri = getRi(color),
						gi = getGi(color),
						bi = getBi(color);

					ASSERT(ri < cntR);
					ASSERT(gi < cntG);
					ASSERT(bi < cntB);
					
					indexTable[color] = blocks[ri][gi][bi].getIndex(&lastIndex);
				}

				// Заполняем таблицу цветов

				colorTable.resize(lastIndex);

				if (skipForeground) {
					colorTable[FOREGROUND_INDEX] = FOREGROUND_COLOR;
				}

				for (int ri = 0; ri < cntR; ++ri) {
					for (int gi = 0; gi < cntG; ++gi) {
						for (int bi = 0; bi < cntB; ++bi) {

							const block& blk = blocks[ri][gi][bi];

							if (blk.inited()) {
								colorTable[blk.getIndex()] = blk.avg();
							}
						}
					}
				}
			}

			
			static inline float zeroIfNaN(float x) {
				return isnan(x) ? 0 : x;
			}


			int getRi(rgb_t color) const {
				int i = min(zeroIfNaN((getR(color) - minR) / lenR), cntR - 1.f);
				ASSERT(i >= 0);
				return i;
			}

			int getGi(rgb_t color) const {
				int i = min(zeroIfNaN((getG(color) - minG) / lenG), cntG - 1.f);
				ASSERT(i >= 0);
				return i;
			}

			int getBi(rgb_t color) const {
				int i = min(zeroIfNaN((getB(color) - minB) / lenB), cntB - 1.f);
				ASSERT(i >= 0);
				return i;
			}
		};


	public:
		void quantize(const uint32_t colors, bool skipForeground) {
			jointChars.clear();

			map<rgb_t, int> pixelMap;
			
			for (int i = 0, s = getWidth() * getHeight(); i < s; ++i) {
				pixelMap[pixel(i)] += 1;
			}

			if (pixelMap.size() <= colors) {
				bool hasForeground = skipForeground && pixelMap.size() > FOREGROUND_INDEX - 1;
				int colorTableSize = pixelMap.size() + (hasForeground ? 2 : 1);

				colorTable.resize(colorTableSize);
				colorTable[BACKGROUND_INDEX] = BACKGROUND_COLOR;

				if (hasForeground) {
					colorTable[FOREGROUND_INDEX] = FOREGROUND_COLOR;
				}

				index_generator i(skipForeground);

				// Использовать pixelMap как indexTable
				for (auto& pixel : pixelMap) {
					pixel.second = i;
					colorTable[i] = pixel.first;
					++i;
				}

				ASSERT_MESSAGE(i == colorTableSize,
						"%d != %d", static_cast<int>(i), colorTableSize);
				
				indexTable = pixelMap;
				isQuantized = false;
				return;
			}

			context ctx(pixelMap, colors, skipForeground);
			
			ctx.initLen();
			ctx.medianCross();
			ctx.calcTables(indexTable, colorTable);

			isQuantized = true;
		}

		bool isQuantized = false;

		bool quantized() const {
			return isQuantized;
		}


	private:
		map<int_pair, chtype> jointChars;

		static constexpr const wchar_t* JOINT_CHARS[] = { L" ", L"▌", L"▐" };

		enum { JCH_SPACE, JCH_LEFT, JCH_RIGHT };

	public:
		void initColors() {
			const vector<rgb_t>& colorTable = getColorTable();

			if (squeezed && jointChars.empty()) {
				map<int_pair, int> jointCounts;

				const map<rgb_t, int>& indexTable = this->indexTable;

				const int endX = getWidth() - 1;

				for (int y = 0, height = getHeight(); y < height; ++y) {
					for (int x = 0; x < endX; x += 2) {
						
						int i1 = indexTable.at(pixel(x, y)),
							i2 = indexTable.at(pixel(x + 1, y));
						
						if (i1 != i2) {
							jointCounts[sortedIntPair(i1, i2)] += 1;
						}
					}
				}

				set<pair<int, int_pair>> sortedJointCounts;

				for (const auto& entry : jointCounts) {
					sortedJointCounts.emplace(entry.second, entry.first);
				}


				const int size = colorTable.size();
				dynamic_bitset usedPairs(size);

				for (const auto& entry : sortedJointCounts) {
					int_pair pair = entry.second;

					int i1 = firstInt(pair),
						i2 = secondInt(pair);
					
					int index;
					int ch1 = JCH_LEFT,
						ch2 = JCH_RIGHT; // Индексы в массиве JOINT_CHARS
					
					if (!usedPairs.at(i1)) {
						index = i1;
						usedPairs[index] = true;
						init_pair(index, i2, i1); // index, fg, bg

					} else if (!usedPairs.at(i2)) {
						index = i2;
						usedPairs[index] = true;
						init_pair(index, i1, i2); // index, fg, bg

					} else {
						index = i1;
						ch1 = ch2 = JCH_SPACE;
					}

					jointChars[pair]                 = ch1 | COLOR_PAIR(index);
					jointChars[reverseIntPair(pair)] = ch2 | COLOR_PAIR(index);
				}

				for (int i = 1; i < size; ++i) {
					if (!usedPairs.at(i)) {
						init_pair(i, i, i);
					}
				}


				for (int index = 1; index < size; ++index) {
					rgb_t color = colorTable[index];

					int ret1 = init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);

					ASSERT_MESSAGE(ret1 == OK, "%d %d", index, color);
				}

			} else {
				for (int index = 1, size = colorTable.size(); index < size; ++index) {
					rgb_t color = colorTable[index];

					int ret1 = init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);
					int ret2 = init_pair(index, index, index);

					ASSERT_MESSAGE(ret1 == OK, "%d %d", index, color);
					ASSERT_MESSAGE(ret2 == OK, "%d", index);
				}
			}
		}


		inline int getStartX() const {
			return (COLS - static_cast<int>(ceil(getWidth() * scaleX()))) / 2;
		}


		inline int getStartY() const {
			return (LINES - getHeight()) / 2;
		}


		inline int getPixelX(int cx) const {
			return min(getWidth(), max(0, static_cast<int>(ceil((cx - getStartX()) / scaleX()))));
		}


		inline int getPixelY(int cy) const {
			return min(getHeight(), max(0, cy - getStartY()));
		}


		void draw() const {
			draw(0, 0, getWidth(), getHeight());
		}
		
		void drawInChars(int sx, int sy, int ex, int ey) const {
			draw(getPixelX(sx - 1) & ~0x1, getPixelY(sy), getPixelX(ex), getPixelY(ey));
		}

		void draw(int sx, int sy, int ex, int ey) const {
			int width = getWidth(),
				dx = getStartX() + static_cast<int>(sx * scaleX()),
				dy = getStartY();
			
			bool sq = squeezed; // Кэшируем

			for (int y = sy; y < ey; ++y) {
				move(dy + y, dx);

				for (int x = sx; x < ex; ++x) {
					int i1 = indexTable.at(pixel(x, y));
					chtype ch = ' ' | COLOR_PAIR(i1);
					addch(ch);
					addch(ch);

					if (sq && (x & 0x1) == 0 && x < width - 1) {
						int i2 = indexTable.at(pixel(x + 1, y));

						if (i1 == i2) {
							addch(ch);
						} else {
							int hc = jointChars.at(intPair(i1, i2));

							attron(hc & A_COLOR);
							addwstr(JOINT_CHARS[hc & 0xFF]);
							attroff(hc & A_COLOR);
						}
					}
				}
			}
		}

	private:
		friend class ImageEntry;

		void swap(int index1, int index2) {
			if (index1 == index2)
				return;

			rgb_t color1 = colorTable[index1],
				  color2 = colorTable[index2];
			
			colorTable[index1] = color2;
			colorTable[index2] = color1;

			for (auto& entry : indexTable) {
				int index = entry.second;

				if (index == index1) {
					entry.second = index2;

				} else if (index == index2) {
					entry.second = index1;
				}
			}
		}
	};
}

#endif /* NCV_QUANTIZIED_IMAGE_CPP */
