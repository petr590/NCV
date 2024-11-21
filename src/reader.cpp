#include "reader.h"
#include "video_reader.cpp"
#include "audio_reader.cpp"
#include "alert.h"

#include "debug.h"

namespace ncv {

	using std::make_unique;

	void readStreams(AVFormatContext* context, AVStream* videoStream, AVStream* audioStream) {
		auto videoReader = videoStream == NULL ? NULL : make_unique<VideoReader>(videoStream);
		auto audioReader = audioStream == NULL ? NULL : make_unique<AudioReader>(audioStream);

		AVPacket* packet = throwIfNull(av_packet_alloc());
		AVFrame* avFrame = throwIfNull(av_frame_alloc());


		for (; !stopped && av_read_frame(context, packet) >= 0; av_packet_unref(packet)) {
			Reader* reader = NULL;

			if (videoStream != NULL && packet->stream_index == videoStream->index)
				reader = videoReader.get();
			
			if (audioStream != NULL && packet->stream_index == audioStream->index)
				reader = audioReader.get();

			if (reader != NULL)
				reader->sendPacket(packet);


			if (videoReader != NULL) videoReader->receiveFrames(packet, avFrame);
			if (audioReader != NULL) audioReader->receiveFrames(packet, avFrame);
		}

		av_frame_free(&avFrame);
		av_packet_free(&packet);
	}


	void readFrameGroups(const File& file) {
		AVFormatContext* context = NULL;

		if (avformat_open_input(&context, file.path().c_str(), NULL, NULL) != 0) {
			alert(L"Could not open the input file: " + file.wpath());
			return;
		};

		throwIfNegative(avformat_find_stream_info(context, NULL));


		AVStream *videoStream = NULL,
				 *audioStream = NULL;

		for (uint i = 0; i < context->nb_streams; ++i) {
			AVStream* stream = context->streams[i];

			switch (stream->codecpar->codec_type) {
				case AVMEDIA_TYPE_VIDEO: if (videoStream == NULL) videoStream = stream; break;
				case AVMEDIA_TYPE_AUDIO: if (audioStream == NULL) audioStream = stream; break;
				default: break;
			}
		}

		readStreams(context, videoStream, audioStream);

		avformat_close_input(&context);
	}
}