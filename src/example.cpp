extern "C" {
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

void processVideoFrame(AVFrame *frame) {
	// ...
}

void processAudioFrame(AVFrame *frame) {
	// ...
}

void readStreams(AVFormatContext *context, AVStream *videoStream, AVStream *audioStream) {
	AVPacket *packet = av_packet_alloc();
	AVFrame *avFrame = av_frame_alloc();

	for (; av_read_frame(context, packet) >= 0; av_packet_unref(packet)) {
		if (videoStream != NULL && packet->stream_index == videoStream->index) {
			processVideoFrame(avFrame);
		}
		
		if (audioStream != NULL && packet->stream_index == audioStream->index) {
			processAudioFrame(avFrame);
		}
	}

	av_frame_free(&avFrame);
	av_packet_free(&packet);
}

int main() {
	AVFormatContext *context = NULL;

	if (avformat_open_input(&context, "file.mp4", NULL, NULL) != 0) {
		fprintf(stderr, "Could not open the input file\n");
		return 1;
	};

	if (avformat_find_stream_info(context, NULL) < 0) {
		fprintf(stderr, "Could not find stream info\n");
		return 1;
	}


	AVStream *videoStream = NULL,
			 *audioStream = NULL;

	for (uint i = 0; i < context->nb_streams; ++i) {
		AVStream *stream = context->streams[i];

		switch (stream->codecpar->codec_type) {
			case AVMEDIA_TYPE_VIDEO: if (videoStream == NULL) videoStream = stream; break;
			case AVMEDIA_TYPE_AUDIO: if (audioStream == NULL) audioStream = stream; break;
			default: break;
		}
	}

	readStreams(context, videoStream, audioStream);
	return 0;
}