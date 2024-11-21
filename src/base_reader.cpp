#ifndef NCV_BASE_READER_CPP
#define NCV_BASE_READER_CPP

#include "reader.h"
#include "throw_if_error.h"

namespace ncv {
	static AVCodecContext* getCodecContext(AVStream* stream) {
		AVCodecParameters* parameters = stream->codecpar;

		AVCodec* codec = throwIfNull(avcodec_find_decoder(parameters->codec_id));

		AVCodecContext* context = throwIfNull(avcodec_alloc_context3(codec));
		throwIfNegative(avcodec_parameters_to_context(context, parameters));
		throwIfNegative(avcodec_open2(context, codec, NULL));

		return context;
	}
	
	class Reader {
	public:
		AVStream* stream;
		AVCodecContext* codecContext;

		Reader(AVStream* stream):
			stream(stream), codecContext(getCodecContext(stream)) {}
		
		virtual ~Reader() {
			avcodec_free_context(&codecContext);
		}

		void sendPacket(AVPacket* packet) {
			throwIfError(avcodec_send_packet(codecContext, packet));
		}

		void receiveFrames(AVPacket* packet, AVFrame* avFrame) {
			while (0 == throwIfErrorExcept(avcodec_receive_frame(codecContext, avFrame), AVERROR(EAGAIN))) {
				process(packet, avFrame);
			}
		};
		
		virtual void process(const AVPacket*, AVFrame*) = 0;
	};
}

#endif /* NCV_BASE_READER_CPP */