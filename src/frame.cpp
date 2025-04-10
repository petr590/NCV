#include "frame.h"
#include "ncurses_wrap.h"
#include "throw_if_error.h"

extern "C" {
	#include <libavutil/imgutils.h>
}

#include "debug.h"

namespace ncv {

	Frame::Frame(AVFrame* frame, AVCodecContext* codecContext, SwsContext* context, int newWidth, int newHeight) {	
		AVFrame* newFrame = throwIfNull(av_frame_alloc());

		av_frame_copy_props(newFrame, frame);

		newFrame->width = newWidth;
		newFrame->height = newHeight;
		newFrame->format = AV_PIX_FMT_RGB24;

		av_frame_get_buffer(newFrame, 0);

		throwIfNegative(sws_scale(context, frame->data, frame->linesize, 0, codecContext->height, newFrame->data, newFrame->linesize));

		this->avFrame = newFrame;
	}

	Frame::~Frame() {
		if (avFrame != nullptr) {
			av_frame_free(&avFrame);
			avFrame = nullptr;
		}
	}

	Frame::Frame(Frame&& other) {
		avFrame = other.avFrame;
		other.avFrame = nullptr;
	}
	
	Frame& Frame::operator=(Frame&& other) {
		this->~Frame();
		avFrame = other.avFrame;
		other.avFrame = nullptr;
		return *this;
	}


	int64_t Frame::pts() const {
		return avFrame->pts;
	}

	
	rgb_t Frame::pixel(int x, int y) const {
		const uint8_t* data = avFrame->data[0];

		int i = (x * BPP + y * avFrame->linesize[0]);

		return getColor(data[i], data[i+1], data[i+2]);
	}

	void Frame::collectColors(map<rgb_t, count_t>& pixelMap) const {
		int w = getWidth(),
			h = getHeight();

		for (int x = 0; x < w; ++x) {
			for (int y = 0; y < h; ++y) {
				pixelMap[pixel(x, y)] += 1;
			}
		}
	}
}
