#include "reader.h"
#include "video_reader.cpp"
#include "audio_reader.cpp"
#include "alert.h"

#include "debug.h"

namespace ncv {

	using std::make_unique;

	static void readStreams(AVFormatContext* context, AVStream* videoStream, AVStream* audioStream) {
		auto videoReader = videoStream == nullptr ? nullptr : make_unique<VideoReader>(videoStream);
		auto audioReader = audioStream == nullptr ? nullptr : make_unique<AudioReader>(audioStream);

		AVPacket* packet = throwIfNull(av_packet_alloc());
		AVFrame* avFrame = throwIfNull(av_frame_alloc());


		for (; !stopped && av_read_frame(context, packet) >= 0; av_packet_unref(packet)) {
			BaseReader* reader = nullptr;

			if (videoStream != nullptr && packet->stream_index == videoStream->index)
				reader = videoReader.get();
			
			if (audioStream != nullptr && packet->stream_index == audioStream->index)
				reader = audioReader.get();

			if (reader != nullptr)
				reader->sendPacket(packet);


			if (videoReader != nullptr) videoReader->receiveFrames(avFrame);
			if (audioReader != nullptr) audioReader->receiveFrames(avFrame);
		}

		av_frame_free(&avFrame);
		av_packet_free(&packet);
	}


	void readFrameGroups(const File& file) {
		AVFormatContext* context = nullptr;

		if (avformat_open_input(&context, file.path().c_str(), nullptr, nullptr) != 0) {
			alert(L"Could not open the input file: " + file.wpath());
			return;
		};

		throwIfNegative(avformat_find_stream_info(context, nullptr));


		AVStream *videoStream = nullptr,
		         *audioStream = nullptr;

		for (uint i = 0; i < context->nb_streams; ++i) {
			AVStream* stream = context->streams[i];

			switch (stream->codecpar->codec_type) {
				case AVMEDIA_TYPE_VIDEO: if (videoStream == nullptr) videoStream = stream; break;
				case AVMEDIA_TYPE_AUDIO: if (audioStream == nullptr) audioStream = stream; break;
				default: break;
			}
		}

		readStreams(context, videoStream, audioStream);

		avformat_close_input(&context);
	}
}
