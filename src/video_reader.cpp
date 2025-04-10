#include "base_reader.cpp"
#include "frame_group.h"
#include "interthread.h"
#include "args.h"
#include "throw_if_error.h"
#include <mutex>

#include "debug.h"

namespace ncv {
	using std::max;
	using std::move;
	using std::mutex;
	using std::lock_guard;

	/* Во сколько раз количество цветов в группе может превышать количество цветов в кадре */
	const int EXCESS_RATIO = 16;

	/* Минимальный размер группы. -1 для снятия ограничения */
	const int MIN_FRAME_GROUP_SIZE = 30;

	/* Максимальный размер группы. -1 для снятия ограничения */
	const int MAX_FRAME_GROUP_SIZE = -1;


	static AVPixelFormat undeprecatePixFormat(AVPixelFormat format) {
		switch (format) {
			case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
			case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
			case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
			case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
			default: return format;
		}
	}


	class VideoReader: public BaseReader {
		FrameGroup group;
		
		SwsContext* swsContext;

		int vptWidth, vptHeight; // Доступные размеры экрана
		int newWidth, newHeight; // Размеры, к которым приводится видео

		size_t colors = 0;
		map<rgb_t, count_t> groupPixelMap;

	public:
		explicit VideoReader(AVStream* stream):
				BaseReader(stream),
				group(stream->time_base) {

			updateSizeAndSws();
		}

		~VideoReader() override {
			if (group.size() > 0) {
				finishGroup();
			}

			sws_freeContext(swsContext);
			swsContext = nullptr;
		}
		
		
		void updateSizeAndSws() {
			AVCodecParameters* parameters = stream->codecpar;
			
			vptWidth = viewportWidthPixels();
			vptHeight = viewportHeightPixels();

			std::tie(newWidth, newHeight) = fitSize(vptWidth, vptHeight, parameters->width, parameters->height);

			swsContext = throwIfNull(sws_getContext(
				parameters->width, parameters->height,
				undeprecatePixFormat(static_cast<AVPixelFormat>(parameters->format)),
				newWidth, newHeight, AV_PIX_FMT_RGB24,
				SWS_BICUBIC, nullptr, nullptr, nullptr
			));
		}
		

		virtual void process(AVFrame* avFrame) override {
			if (stopped) {
				group.clear();
				return;
			}
			
			Frame frame(avFrame, codecContext, swsContext, newWidth, newHeight);

			map<rgb_t, count_t> pixelMap;
			frame.collectColors(pixelMap);

			if constexpr (MAX_FRAME_GROUP_SIZE > 0) {
				if (group.size() >= MAX_FRAME_GROUP_SIZE) {
					finishGroup();
					checkSizeUpdated(pixelMap, frame, avFrame);
					newGroup(pixelMap);
				}
			}
			
			// Добавляем пиксели в статистику группы фреймов
			for (const auto& entry : pixelMap) {
				groupPixelMap[entry.first] += entry.second;
			}

			// Инициализируем colors в первый раз
			if (colors == 0) {
				colors = max(groupPixelMap.size(), static_cast<size_t>(COLORS));
			}


			// Если цветов стало слишком много
			if (parallel &&
				group.size() >= MIN_FRAME_GROUP_SIZE &&
				groupPixelMap.size() > colors * EXCESS_RATIO) {
				
				// То вернём статистику как было без нового кадра
				for (const auto& entry : pixelMap) {
					if ((groupPixelMap[entry.first] -= entry.second) == 0) {
						groupPixelMap.erase(entry.first);
					}
				}
				
				finishGroup();
				checkSizeUpdated(pixelMap, frame, avFrame);
				newGroup(pixelMap);
			}

			// В конце концов, перемещаем кадр в группу
			group.add(move(frame));

			frameGroupsProcessPercent = static_cast<int>(100 * avFrame->pts / stream->duration);

			av_frame_unref(avFrame);
		}

		void finishGroup() {
			group.quantize(move(groupPixelMap));
			group.initJoins();

			// Перемещаем группу в общую очередь
			lock_guard<mutex> lock(frameGroupsMutex);
			frameGroups.push_back(move(group));
		}

		void checkSizeUpdated(map<rgb_t, count_t>& pixelMap, Frame& frame, AVFrame* avFrame) {
			if (vptWidth != viewportWidthPixels() ||
				vptHeight != viewportHeightPixels()) {
				
				sws_freeContext(swsContext);
				swsContext = nullptr;
				updateSizeAndSws();
				
				// Обновляем фрейм и пиксельмапу, так как поменялся размер
				frame = move(Frame(avFrame, codecContext, swsContext, newWidth, newHeight));
				
				pixelMap.clear();
				frame.collectColors(pixelMap);
			}
		}

		void newGroup(map<rgb_t, count_t>& pixelMap) {
			group.setTimeBase(stream->time_base);
			groupPixelMap = move(pixelMap);
			colors = max(groupPixelMap.size(), static_cast<size_t>(COLORS));
		}
	};
}
