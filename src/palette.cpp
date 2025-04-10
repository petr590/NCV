#include "palette.h"
#include "ncv_assert.h"
#include "dynamic_bitset.h"
#include <set>

#include "debug.h"

namespace ncv {
	using std::map;
	using std::set;
	using std::pair;
	using std::min;
	using std::max;

	struct Palette::index_generator {
	private:
		const bool skipForeground;
		index_t value = BACKGROUND_INDEX + 1;

	public:
		explicit index_generator(bool skipForeground):
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

		inline operator index_t() const {
			return value;
		}
	};


	/** Один блок в rgb кубе */
	struct Palette::block {
		uint64_t r = 0,
				 g = 0,
				 b = 0,
				 c = 0;

	private:
		index_t index = -1;
		
		// Блок, объединённый с этим. Работают вместе как один блок
		block* other = nullptr;
		
	public:
		block() {}

		inline bool inited() const {
			return index != -1;
		}
		
		rgb_t avg() const {
			return c == 0 ? -1 : getColor(
				static_cast<int>(r / c),
				static_cast<int>(g / c),
				static_cast<int>(b / c)
			);
		}

		index_t getIndex() const {
			return index;
		}

		index_t getIndex(index_generator* last) {
			if (index != -1)
				return index;
			
			index_t res = index = (*last)++;

			if (other != nullptr)
				other->index = res;

			return res;
		}

		bool joined() const {
			return other != nullptr;
		}

		void join(block* other) {
			ASSERT(!this->joined() && !other->joined());

			this->other = other;
			other->other = this;

			this->r = other->r = this->r + other->r;
			this->g = other->g = this->g + other->g;
			this->b = other->b = this->b + other->b;
			this->c = other->c = this->c + other->c;
		}
	};


	struct Palette::context {
	private:
		const map<rgb_t, count_t>& pixelMap; // Ключ: цвет, значение: количество в изображении

		const uint32_t colors; // Максимальное количество цветов после квантизации 

		const bool skipForeground; // Скипать или нет FOREGROUND_INDEX в таблицах

		int   minR = 0xFF, minG = 0xFF, minB = 0xFF;
		int   cntR = 1,    cntG = 1,    cntB = 1;
		float lenR = 0,    lenG = 0,    lenB = 0;

		uint32_t filledBlocks = -1; // Кэшируем, чтобы не считать ещё раз
		vector<vector<vector<block>>> blocks;

		inline bool maximizeColors() {
			return colors <= 256;
		}

		inline int colorsMultiplier() {
			return maximizeColors() ? 1 : 2;
		}

	public:
		context(const map<rgb_t, count_t>& pixelMap, uint32_t colors, bool skipForeground):
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
			for (uint32_t parts = colorsMultiplier(); parts < colors || checkHasPlace(); parts *= 2) {
				float maxLen = max(lenR * RM, max(lenG * GM, lenB * BM));
				
				if (maxLen == lenR * RM)
					lenR /= 2, cntR *= 2;
				else if (maxLen == lenG * GM)
					lenG /= 2, cntG *= 2;
				else
					lenB /= 2, cntB *= 2;
			}
		}


		#define USE_BITSET 1

		bool checkHasPlace() {
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
			filledBlocks = static_cast<uint32_t>(used.count());
			#else
			filledBlocks = static_cast<uint32_t>(used.size());
			#endif

			return filledBlocks * colorsMultiplier() <= colors;
		}

		#undef USE_BITSET

		
		void calcTables(map<rgb_t, index_t>& indexTable, vector<rgb_t>& colorTable) {

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

				uint64_t c = static_cast<uint64_t>(pixel.second);
				
				blk.r += getR(color) * c;
				blk.g += getG(color) * c;
				blk.b += getB(color) * c;
				blk.c += c;
			}


			if (maximizeColors()) {
				// Количество цветов может быть больше, чем colors
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


		void joinBlocks() {
			uint32_t filled = filledBlocks;

			// Ищем пару соседних блоков с минимальным кол-вом цветов и объединяем
			if (filled > colors) {
				// Таблица смещений координат
				const int offsets[3][3] = {
				//   r g b
					{1,0,0},
					{0,1,0},
					{0,0,1},
				};

				set<pair<uint64_t, pair<block*, block*>>> pairs;
				
				for (int ri = 0; ri < cntR; ++ri) {
					for (int gi = 0; gi < cntG; ++gi) {
						for (int bi = 0; bi < cntB; ++bi) {
							block& blk1 = blocks[ri][gi][bi];

							uint64_t c = blk1.c;

							if (c == 0)
								continue;

							for (const int* offset : offsets) {
								int ri2 = ri + offset[0],
									gi2 = gi + offset[1],
									bi2 = bi + offset[2];
								
								if (ri2 < cntR && gi2 < cntG && bi2 < cntB) {
									block& blk2 = blocks[ri2][gi2][bi2];

									if (blk2.c == 0)
										continue;
									
									pairs.emplace(blk2.c + c, pair<block*, block*>(&blk1, &blk2));
								}
							}
						}
					}
				}

				for (const auto& entry : pairs) {
					const pair<block*, block*>& p = entry.second;
					
					if (!p.first->joined() && !p.second->joined()) {
						p.first->join(p.second);
						filled -= 1;
						
						if (filled <= colors)
							break;
					}
				}

				ASSERT_MESSAGE(filled <= colors,
						"filled: %u, colors: %u, pairs: %zu of %zu",
						filled, colors, pairs.size(), static_cast<size_t>(cntR) * cntG * cntB);
			}
		}

		
		static float divSafe(float x, float d) {
			return d == 0 ? 0 : x / d;
		}


		int getRi(rgb_t color) const {
			int i = static_cast<int>(min(divSafe(getR(color) - minR, lenR), cntR - 1.f));
			ASSERT(i >= 0);
			return i;
		}

		int getGi(rgb_t color) const {
			int i = static_cast<int>(min(divSafe(getG(color) - minG, lenG), cntG - 1.f));
			ASSERT(i >= 0);
			return i;
		}

		int getBi(rgb_t color) const {
			int i = static_cast<int>(min(divSafe(getB(color) - minB, lenB), cntB - 1.f));
			ASSERT(i >= 0);
			return i;
		}
	};

	void Palette::quantize(map<rgb_t, count_t>&& pixelMap, uint32_t colors, bool skipForeground) {
		// Если количество цветов не превышает максимальное,
		// то вообще не квантизируем палитру
		if (pixelMap.size() <= colors) {

			bool hasForeground = skipForeground && pixelMap.size() > FOREGROUND_INDEX - 1;
			size_t colorTableSize = pixelMap.size() + (hasForeground ? 2 : 1);

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

			ASSERT_MESSAGE(static_cast<size_t>(i) == colorTableSize,
					"%zu != %zu", static_cast<size_t>(i), colorTableSize);
			
			indexTable = move(pixelMap);
			isQuantized = false;
			return;
		}

		context ctx(pixelMap, colors, skipForeground);
		
		ctx.initLen();
		ctx.medianCross();
		ctx.calcTables(indexTable, colorTable);

		isQuantized = true;
	}


	void Palette::swapIndexes(index_t index1, index_t index2) {
		if (index1 == index2)
			return;

		rgb_t color1 = colorTable[index1],
			  color2 = colorTable[index2];
		
		colorTable[index1] = color2;
		colorTable[index2] = color1;

		for (auto& entry : indexTable) {
			index_t index = entry.second;

			if (index == index1) {
				entry.second = index2;

			} else if (index == index2) {
				entry.second = index1;
			}
		}
	}
}
