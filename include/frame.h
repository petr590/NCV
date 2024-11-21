#ifndef NCV_FRAME_H
#define NCV_FRAME_H

extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

#include "rgb.h"
#include <map>
#include <memory>

namespace ncv {
	using std::map;
	using std::unique_ptr;

	class Frame {
		static const int BPP = 3;

		AVFrame* avFrame = NULL;

	public:
		Frame(AVFrame*, AVCodecContext*, SwsContext*, int newWidth, int newHeight);

		~Frame();

		Frame(const Frame&) = delete;

		Frame(Frame&&);
		
		Frame& operator=(Frame&&);

		inline int getWidth() const {
			return avFrame->width;
		}

		inline int getHeight() const {
			return avFrame->height;
		}

		int64_t pts() const;

		rgb_t pixel(int x, int y) const;

		void collectColors(map<rgb_t, count_t>& pixelMap) const;
	};
}

#endif /* NCV_FRAME_H */