#include "frame_group.h"
#include "interthread.h"
#include "ncv_assert.h"
#include "pair_utils.h"
#include "dynamic_bitset.h"
#include "args.h"
#include <set>
#include <chrono>
#include <thread>
#include <algorithm>

#include "debug.h"

namespace ncv {
	using std::swap;
	using std::pair;
	using std::set;
	using std::mutex;
	using std::lock_guard;

	static bool skipForeground() {
		return COLORS > 8;
	}

	static int maxColors() {
		return COLORS - (skipForeground() ? 2 : 1);
	}

	static int maxColorPairs() {
		return COLOR_PAIRS - 1;
	}

	FrameGroup::FrameGroup(const AVRational& time_base):
			time_base(time_base) {

		ASSERT(time_base.den != 0);
	}
	
	FrameGroup::FrameGroup(FrameGroup&& other):
		frames          (std::move(other.frames)),
		palette         (std::move(other.palette)),
		joins           (std::move(other.joins)),
		jointIndexTable (std::move(other.jointIndexTable)),
		time_base       (std::move(other.time_base))
	{
		ASSERT(time_base.den != 0);
	}

	void FrameGroup::setTimeBase(const AVRational& time_base) {
		this->time_base = time_base;
	}

	size_t FrameGroup::size() const {
		return frames.size();
	}

	void FrameGroup::add(Frame&& frame) {
		if (!frames.empty()) {
			ASSERT(frames[0].getWidth() == frame.getWidth());
			ASSERT(frames[0].getHeight() == frame.getHeight());
		}

		frames.push_back(std::move(frame));
	}

	int FrameGroup::getWidth() const {
		return frames.empty() ? 0 : frames[0].getWidth();
	}

	int FrameGroup::getHeight() const {
		return frames.empty() ? 0 : frames[0].getHeight();
	}

	void FrameGroup::quantize(map<rgb_t, count_t>&& pixelMap) {
		quantize(std::move(pixelMap), maxColors(), skipForeground());
	}

	void FrameGroup::quantize(map<rgb_t, count_t>&& pixelMap, uint32_t colors, bool skipForeground) {
		palette.quantize(std::move(pixelMap), colors, skipForeground);
	}


	const int
			JCH_SHIFT = 16,
			JCH_SPACE = 0 << JCH_SHIFT,
			JCH_UPPER = 1 << JCH_SHIFT;

	
	/**
	 * @return список пар, отсортированный по первому значению в порядке возрастания.
	 * Первое значение - сколько раз эта пара индексов встречается в изображении,
	 * второе - пара индексов, не равных друг другу.
	 */
	void FrameGroup::initJoins() {
		if (!doubleResolution)
			return;

		map<index_pair, count_t> joinsMap;

		const map<rgb_t, index_t>& indexTable = palette.getIndexTable();

		const int endX = getWidth();
		const int endY = getHeight() - 1;

		for (const Frame& frame : frames) {
			for (int y = 0; y < endY; y += 2) {
				for (int x = 0; x < endX; x += 1) {
					
					index_t i1 = indexTable.at(frame.pixel(x, y)),
							i2 = indexTable.at(frame.pixel(x, y + 1));
					
					if (i1 != i2) {
						joinsMap[sorted_pair(i1, i2)] += 1;
					}
				}
			}
		}

		joins.reserve(joinsMap.size());
		
		transform(joinsMap.begin(), joinsMap.end(), back_inserter(joins),
				[] (const auto& p) { return make_pair(p.second, p.first); });

		sort(joins.begin(), joins.end());
	}


	void FrameGroup::initColors() {
		if (!doubleResolution) {
			lock_guard<mutex> lock(ncursesMutex);
			
			// Инициализируем цвета и пары
			initNcursesColors(palette.getColorTable(), true);
			return;
		}


		const count_t colorsCount = static_cast<count_t>(palette.getColorTable().size());

		set<index_t> colorIndexes;
		for (index_t i = 1; i < colorsCount; ++i)
			colorIndexes.insert(i);
		
		count_t space = maxColorPairs() * 2;
		count_t remains = colorsCount;
		cp_index_t lastCpIndex = 0;

		// Ключ - индекс цвета, значение - индекс пары ncurses и флаги по индексу цвета
		map<index_t, int32_t> oneColorIndexTable;

		// Джойны, которым не хватило места
		vector<index_pair> uninitialized;

		lock_guard<mutex> lock(ncursesMutex);

		// Инициализируем джойны, которые влезают
		for (const auto& entry : joins) {
			index_pair pair = entry.second;

			index_t i1 = pair.first,
					i2 = pair.second;
			
			count_t d = (colorIndexes.find(i1) != colorIndexes.end()) +
						(colorIndexes.find(i2) != colorIndexes.end());

			if (space - 2 >= remains - d) {
				space -= 2;

				if (d != 0) {
					remains -= d;
					colorIndexes.erase(i1);
					colorIndexes.erase(i2);
				}

				lastCpIndex += 1;

				jointIndexTable[pair]                = JCH_UPPER | EXT_COLOR_PAIR(lastCpIndex);
				jointIndexTable[reversed_pair(pair)] = JCH_UPPER | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;

				jointIndexTable[index_pair(i1, i1)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
				jointIndexTable[index_pair(i2, i2)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

				oneColorIndexTable[i1] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
				oneColorIndexTable[i2] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

				int ret = init_extended_pair(lastCpIndex, i1, i2); // cp index, fg, bg
				ASSERT_MESSAGE(ret == OK, "%d %d %d", lastCpIndex, i1, i2);

			} else {
				uninitialized.push_back(pair);
			}
		}

		
		// Делаем чётное количество
		if ((colorIndexes.size() & 0x1) == 1) {
			colorIndexes.insert(0);
		}

		// И объединяем по парам оставшиеся цвета
		for (auto it = colorIndexes.begin(); it != colorIndexes.end(); ) {
			index_t i1 = *(it++),
					i2 = *(it++);
			
			lastCpIndex += 1;

			int ret = init_extended_pair(lastCpIndex, i1, i2); // cp index, fg, bg
			ASSERT_MESSAGE(ret == OK, "%d %d %d", lastCpIndex, i1, i2);

			jointIndexTable[index_pair(i1, i1)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
			jointIndexTable[index_pair(i2, i2)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

			oneColorIndexTable[i1] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
			oneColorIndexTable[i2] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);
		}

		// Инициализируем оставшиеся пары
		for (index_pair pair : uninitialized) {
			jointIndexTable[pair]                = oneColorIndexTable.at(pair.first);
			jointIndexTable[reversed_pair(pair)] = oneColorIndexTable.at(pair.second);
		}

		// Инициализируем цвета
		initNcursesColors(palette.getColorTable(), false);
	}


	void FrameGroup::initNcursesColors(const vector<rgb_t>& colorTable, bool initPairs) const {
		for (size_t index = 1, size = colorTable.size(); index < size; ++index) {
			rgb_t color = colorTable.at(index);

			int ret1 = init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);
			ASSERT_MESSAGE(ret1 == OK, "%zu %d", index, color);

			if (initPairs) {
				int ret2 = init_pair(index, index, index); // cp index, fg, bg
				ASSERT_MESSAGE(ret2 == OK, "%zu", index);
			}
		}
	}


	int FrameGroup::getStartX() const {
		return (COLS - static_cast<int>(ceil(getWidth() * scaleX()))) / 2;
	}

	int FrameGroup::getStartY() const {
		return (LINES - static_cast<int>(ceil(getHeight() * scaleY()))) / 2;
	}

	void FrameGroup::draw(const File& file) const {
		draw(file, 0, 0, getWidth() - 1, getHeight() - 1);
	}


	using namespace std::chrono;
	namespace this_thread = std::this_thread;

	static steady_clock::time_point lastTimestamp;
	static int64_t lastPts = 0;


	void FrameGroup::resetTimestamps() {
		lastTimestamp = {};
		lastPts = 0;
	}


	void FrameGroup::sleepUntilFrame(const Frame& frame) const {
		int64_t pts = frame.pts();
		ASSERT_MESSAGE(pts >= lastPts, "%ld < %ld", pts, lastPts);

		auto waiting = nanoseconds(nanoseconds::period::den * (pts - lastPts) * time_base.num / time_base.den);

		ASSERT_MESSAGE(waiting >= 0ns,
				"Overflow: %ld * %ld * %d",
				nanoseconds::period::den, pts - lastPts, time_base.num
		);

		this_thread::sleep_for(max(waiting - (steady_clock::now() - lastTimestamp), 0ns));

		lastTimestamp = steady_clock::now();
		lastPts = pts;
	}


	void FrameGroup::draw(const File& file, int sx, int sy, int ex, int ey) const {
		int dx = getStartX() + static_cast<int>(sx * scaleX()),
			dy = getStartY() + static_cast<int>(sy * scaleY());
		
		const bool dbRes = doubleResolution; // Кэшируем
		const map<rgb_t, index_t>& indexTable = palette.getIndexTable();

		const Frame* prev1 = nullptr;
		const Frame* prev2 = nullptr;

		for (const Frame& frame : frames) {
			if (stopped)
				return;
			
			TIMER(t, "total draw");
			WINDOW* const buffer = drawBuffer1;

			if (!dbRes) {
				map<rgb_t, vector<pair<int, int>>> colorCoords;

				{
					TIMER(t1, "colorCoords");

					for (int y = sy; y <= ey; ++y) {
						for (int x = sx; x <= ex; ++x) {
							rgb_t pix = frame.pixel(x, y);

							if (prev2 != nullptr && pix == prev2->pixel(x, y)) {
								continue;
							}

							colorCoords[pix].emplace_back(x, y);
						}
					}
				}

				sleepUntilFrame(frame);

				TIMER(t2, "ncurses draw");

				for (const auto& colorAndCoords : colorCoords) {
					chtype colorPair = COLOR_PAIR(indexTable.at(colorAndCoords.first));

					wattron(buffer, colorPair);

					for (const auto& coord : colorAndCoords.second) {
						wmove(buffer, dy + coord.second, dx + coord.first * 2);
						waddch(buffer, ' ');
						waddch(buffer, ' ');
					}

					wattroff(buffer, colorPair);
				}

			} else {
				cchar_t space = { .attr = 0, .chars = { L' ', L'\0' }, .ext_color = 0 },
						upper = { .attr = 0, .chars = { L'▀', L'\0' }, .ext_color = 0 };

				sleepUntilFrame(frame);

				TIMER(t1, "ncurses draw");

				for (int y = sy; y <= ey-1; y += 2) {
					for (int x = sx; x <= ex; x += 1) {
						rgb_t pix1 = frame.pixel(x, y),
							  pix2 = (y + 1 >= ey) ? RGB_NONE : frame.pixel(x, y + 1);

						if (prev2 != nullptr &&
							pix1 == prev2->pixel(x, y) &&
							(pix2 == RGB_NONE || pix2 == prev2->pixel(x, y + 1))) {
							continue;
						}

						index_t i1 = indexTable.at(pix1),
								i2 = pix2 == RGB_NONE ? i1 : indexTable.at(pix2);

						wmove(buffer, dy + y / 2, dx + x);

						int32_t jch = jointIndexTable.at(index_pair(i1, i2));

						cchar_t* cchar = (jch & JCH_UPPER) != 0 ? &upper : &space;

						cchar->ext_color = jch & EXT_COLOR_MASK;
						cchar->attr = jch & A_REVERSE;

						wadd_wch(buffer, cchar);
					}
				}
			}

			prev2 = prev1;
			prev1 = &frame;
			file.printToWindow(buffer);

			lock_guard<mutex> lock(ncursesMutex);
			swap(drawBuffer1, drawBuffer2);
			updateStdscr = true;
		}
	}

	void FrameGroup::clear() {
		frames.clear();
	}
}
