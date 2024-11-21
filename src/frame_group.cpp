#include "frame_group.h"
#include "interthread.h"
#include "ncv_assert.h"
#include "dynamic_bitset.h"
#include <set>
#include <chrono>
#include <thread>
#include <algorithm>

#include "debug.h"

namespace ncv {
	using std::pair;
	using std::set;

	static int maxColors() {
		return COLORS - 2;
	}

	static int maxColorPairs() {
		return COLOR_PAIRS - 1;
	}

	static const bool SKIP_FOREGROUND = true;

	FrameGroup::FrameGroup(const AVRational& time_base):
			time_base(time_base) {

		ASSERT(time_base.den != 0);
	}
	
	FrameGroup::FrameGroup(FrameGroup&& other) {
		frames = std::move(other.frames);
		palette = std::move(other.palette);
		joins = std::move(other.joins);
		jointPixels = std::move(other.jointPixels);
		time_base = std::move(other.time_base);

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
		quantize(std::move(pixelMap), maxColors(), SKIP_FOREGROUND);
	}

	void FrameGroup::quantize(map<rgb_t, count_t>&& pixelMap, uint32_t colors, bool skipForeground) {
		palette.quantize(std::move(pixelMap), colors, skipForeground);
	}


	const int
			JCH_SHIFT = 31,
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
						joinsMap[sortedIntPair(i1, i2)] += 1;
					}
				}
			}
		}

		joins.reserve(joinsMap.size());

		for (const auto& entry : joinsMap) {
			joins.emplace_back(entry.second, entry.first);
		}

		sort(joins.begin(), joins.end());
	}


	void FrameGroup::initColors() {
		if (doubleResolution) {
			const count_t colorsCount = static_cast<count_t>(palette.getColorTable().size());

			set<index_t> colorIndexes;

			for (index_t i = 1; i < colorsCount; ++i)
				colorIndexes.insert(i);
			
			count_t space = maxColorPairs() * 2;
			count_t remains = colorsCount;
			cp_index_t lastCpIndex = 0;

			map<index_t, chtype> chtypeByIndex; // Конкретный символ по индексу цвета
			vector<index_pair> uninitialized; // Джойны, которым не хватило места

			// Инициализируем джойны, которые влезают
			for (const auto& entry : joins) {
				index_pair pair = entry.second;

				index_t i1 = firstInt(pair),
						i2 = secondInt(pair);
				
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

					jointPixels[pair]                 = JCH_UPPER | EXT_COLOR_PAIR(lastCpIndex);
					jointPixels[reverseIntPair(pair)] = JCH_UPPER | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;

					jointPixels[intPair(i1, i1)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
					jointPixels[intPair(i2, i2)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

					chtypeByIndex[i1] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
					chtypeByIndex[i2] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

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

				jointPixels[intPair(i1, i1)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
				jointPixels[intPair(i2, i2)] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);

				chtypeByIndex[i1] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex) | A_REVERSE;
				chtypeByIndex[i2] = JCH_SPACE | EXT_COLOR_PAIR(lastCpIndex);
			}

			// Инициализируем оставшиеся пары
			for (int_pair pair : uninitialized) {
				jointPixels[pair]                 = chtypeByIndex.at(firstInt(pair));
				jointPixels[reverseIntPair(pair)] = chtypeByIndex.at(secondInt(pair));
			}


			const vector<rgb_t>& colorTable = palette.getColorTable();

			// Инициализируем цвета
			for (index_t index = 1; index < colorsCount; ++index) {
				rgb_t color = colorTable[index];

				int ret = init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);
				ASSERT_MESSAGE(ret == OK, "%d %d", index, color);
			}

		} else {
			const vector<rgb_t>& colorTable = palette.getColorTable();

			// Инициализируем цвета и пары
			for (size_t index = 1, size = colorTable.size(); index < size; ++index) {
				rgb_t color = colorTable[index];

				int ret1 = init_color(index, getR(color) * 1000 / 255, getG(color) * 1000 / 255, getB(color) * 1000 / 255);
				int ret2 = init_pair(index, index, index); // cp index, fg, bg

				ASSERT_MESSAGE(ret1 == OK, "%ld %d", index, color);
				ASSERT_MESSAGE(ret2 == OK, "%ld", index);
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

	static milliseconds lastTimestamp;
	static int64_t lastPts = 0;


	void FrameGroup::sleepUntilFrame(const Frame& frame) const {
		int64_t pts = frame.pts();

		milliseconds waiting((pts - lastPts) * 1000 * time_base.num / time_base.den);
		milliseconds timestamp(duration_cast<milliseconds>(system_clock::now().time_since_epoch()));

		this_thread::sleep_for(max(waiting - (timestamp - lastTimestamp), milliseconds(0)));

		lastTimestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		lastPts = pts;
	}


	void FrameGroup::draw(const File& file, int sx, int sy, int ex, int ey) const {
		int dx = getStartX() + static_cast<int>(sx * scaleX()),
			dy = getStartY() + static_cast<int>(sy * scaleY());
		
		const bool db = doubleResolution; // Кэшируем
		const map<rgb_t, index_t>& indexTable = palette.getIndexTable();

		const Frame* prev = nullptr;

		for (const Frame& frame : frames) {
			if (stopped)
				return;


			if (!db) {
				map<rgb_t, vector<pair<int, int>>> colorCoords;

				for (int y = sy; y <= ey; ++y) {
					for (int x = sx; x <= ex; ++x) {
						rgb_t pix = frame.pixel(x, y);

						if (prev != nullptr && pix == prev->pixel(x, y)) {
							continue;
						}

						colorCoords[pix].emplace_back(x, y);
					}
				}

				sleepUntilFrame(frame);

				for (const auto& colorAndCoords : colorCoords) {
					chtype colorPair = COLOR_PAIR(indexTable.at(colorAndCoords.first));

					attron(colorPair);

					for (const auto& coord : colorAndCoords.second) {
						move(dy + coord.second, dx + coord.first * 2);
						addch(' ');
						addch(' ');
					}

					attroff(colorPair);
				}

			} else {
				cchar_t space = { .attr = 0, .chars = { L' ', L'\0' }, .ext_color = 0 },
						upper = { .attr = 0, .chars = { L'▀', L'\0' }, .ext_color = 0 };

				sleepUntilFrame(frame);

				for (int y = sy; y <= ey-1; y += 2) {
					for (int x = sx; x <= ex; x += 1) {
						rgb_t pix1 = frame.pixel(x, y),
							  pix2 = (y + 1 >= ey) ? RGB_NONE : frame.pixel(x, y + 1);

						if (prev != nullptr &&
							pix1 == prev->pixel(x, y) &&
							(pix2 == RGB_NONE || pix2 == prev->pixel(x, y + 1))) {
							continue;
						}

						index_t i1 = indexTable.at(pix1),
								i2 = pix2 == RGB_NONE ? i1 : indexTable.at(pix2);

						move(dy + y / 2, dx + x);

						chtype hc = jointPixels.at(intPair(i1, i2));

						cchar_t* cchar = (hc & JCH_UPPER) != 0 ? &upper : &space;

						cchar->ext_color = hc & EXT_COLOR_MASK;
						cchar->attr = hc & A_REVERSE;

						add_wch(cchar);
					}
				}
			}

			prev = &frame;

			mvprintw(LINES - 1, 0,
					"%ls, %.2f %s",
					file.wpath().c_str(), file.normalizedSize(), file.sizeUnit()
			);
			refresh();
		}
	}

	void FrameGroup::clear() {
		frames.clear();
	}
}