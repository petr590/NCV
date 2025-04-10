#ifndef NCV_FRAME_GROUP_H
#define NCV_FRAME_GROUP_H

#include "palette.h"
#include "frame.h"
#include "files.h"
#include <future>

namespace ncv {
	class FrameGroup {
		std::vector<Frame> frames;
		Palette palette;

		// Данные, кешируемые для отрисовки двух пикселеё в одном символе

		/**
		 * Список пар, отсортированных по первому значению в порядке возрастания.
		 * Первое значение - сколько раз эта пара индексов встречается в изображении,
		 * второе - пара индексов, не равных друг другу.
		 */
		std::vector<std::pair<count_t, index_pair>> joins;

		/**
		 * Ключ - пара индексов цветов.
		 * Значение - индекс пары ncurses и флаги для данной пары индексов цветов.
		 */
		std::map<index_pair, int32_t> jointIndexTable;
		
		AVRational time_base;

	public:
		explicit FrameGroup(const AVRational& time_base);

		FrameGroup(FrameGroup&&);

		void setTimeBase(const AVRational& time_base);

		size_t size() const;

		void add(Frame&&);

		int getWidth() const;

		int getHeight() const;

		void initJoins();

		void initColors();

		void quantize(map<rgb_t, count_t>&& pixelMap);

		void quantize(map<rgb_t, count_t>&& pixelMap, uint32_t colors, bool skipForeground);

	private:
		void initNcursesColors(const std::vector<rgb_t>& colorTable, bool initPairs) const;

		int getStartX() const;
		int getStartY() const;

		void sleepUntilFrame(const Frame&) const;

	public:
		/**
		 * Отрисовывает группу кадров полностью.
		 * Может занимать много времени из-за ожидания.
		 */
		void draw(const File&) const;

		/**
		 * Отрисовывает группу кадров от (sx, sy) включительно до (ex, ey) невключительно.
		 * Может занимать много времени из-за ожидания.
		 */
		void draw(const File&, int sx, int sy, int ex, int ey) const;

		void clear();

		static void resetTimestamps();
	};
}

#endif /* NCV_FRAME_GROUP_H */
