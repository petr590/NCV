#ifndef NCV_BASE_READER_CPP
#define NCV_BASE_READER_CPP

#include "reader.h"
#include "throw_if_error.h"

namespace ncv {
	static AVCodecContext* getCodecContext(const AVStream* stream) {
		AVCodecParameters* parameters = stream->codecpar;

		AVCodec* codec = throwIfNull(avcodec_find_decoder(parameters->codec_id));

		AVCodecContext* context = throwIfNull(avcodec_alloc_context3(codec));
		throwIfNegative(avcodec_parameters_to_context(context, parameters));
		throwIfNegative(avcodec_open2(context, codec, nullptr));

		return context;
	}
	
	class BaseReader {
	public:
		AVStream* stream;
		AVCodecContext* codecContext;

		BaseReader(AVStream* stream):
			stream(stream), codecContext(getCodecContext(stream)) {}
		
		virtual ~BaseReader() {
			avcodec_free_context(&codecContext);
		}

		void sendPacket(AVPacket* packet) {
			throwIfError(avcodec_send_packet(codecContext, packet));
		}

		void receiveFrames(AVFrame* avFrame) {
			while (0 == throwIfErrorExcept(avcodec_receive_frame(codecContext, avFrame), AVERROR(EAGAIN))) {
				process(avFrame);
			}
		}
		
		virtual void process(AVFrame*) = 0;
	};
}

#endif /* NCV_BASE_READER_CPP */
