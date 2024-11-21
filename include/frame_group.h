#ifndef NCV_FRAME_GROUP_H
#define NCV_FRAME_GROUP_H

#include "palette.h"
#include "frame.h"
#include "files.h"
#include <future>

namespace ncv {
	typedef int_pair index_pair;


	class FrameGroup {
		std::vector<Frame> frames;
		Palette palette;

		// Данные, кешируемые для отрисовки двух пикселеё в одном символе

		/**
		 * список пар, отсортированный по первому значению в порядке возрастания.
		 * Первое значение - сколько раз эта пара индексов встречается в изображении,
		 * второе - пара индексов, не равных друг другу.
		 */
		std::vector<std::pair<count_t, index_pair>> joins;

		/**
		 * 
		 */
		std::map<index_pair, chtype> jointPixels;
		
		AVRational time_base;

	public:
		FrameGroup(const AVRational& time_base);

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
	};
}

#endif /* NCV_FRAME_GROUP_H */