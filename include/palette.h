#ifndef NCV_PALETTE_H
#define NCV_PALETTE_H

#include "ncurses_wrap.h"
#include "rgb.h"
#include <vector>
#include <map>

namespace ncv {
	const int NOISE_AMPLITUDE = 3;

	// Нулевая цветовая пара всегда использует эти индексы, её нельзя изменить
	const index_t BACKGROUND_INDEX = COLOR_BLACK,
				  FOREGROUND_INDEX = COLOR_WHITE;
	
	const rgb_t BACKGROUND_COLOR = 0x000000,
				FOREGROUND_COLOR = 0xFFFFFF;
	

	class Palette {
		std::map<rgb_t, index_t> indexTable;
		std::vector<rgb_t> colorTable;

		bool isQuantized = false;

	public:
		/**
		 * @return таблицу цветов. Ключ - индекс цвета, значение - цвет,
		 * который в итоге будет отображён на экране.
		 */
		inline const std::vector<rgb_t>& getColorTable() const {
			return colorTable;
		}

		/**
		 * @return таблицу индексов. Ключ - исходный цвет, значение - соответствующий ему индекс.
		 * Нескольким цветам может соответствовать один индекс.
		 */
		inline const std::map<rgb_t, index_t>& getIndexTable() const {
			return indexTable;
		}

	private:
		struct index_generator;
		struct block;
		struct context;

	public:
		/**
		 * Производит квантизацию цветов.
		 * @param pixelMap таблица цветов. Ключ - цвет, значение - количество в изображении.
		 * @param colors итоговое количество цветов в палитре (их может быть и меньше).
		 * @param skipForeground нужно ли пропускать FOREGROUND_INDEX в таблице индексов.
		 */
		void quantize(std::map<rgb_t, count_t>&& pixelMap, uint32_t colors, bool skipForeground);

		/**
		 * @return true, если палитра была квантизировано,
		 * т.е. количество цветов уменьшилось.
		 */
		bool quantized() const {
			return isQuantized;
		}

		/** Меняет местами индексы в таблице индексов и цветов */
		void swapIndexes(index_t index1, index_t index2);
	};
}

#endif /* NCV_PALETTE_H */