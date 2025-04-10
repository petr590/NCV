#include "base_reader.cpp"

// extern "C" {
// 	#include <libavcodec/avcodec.h>
// 	#include <libswscale/swscale.h>
// 	#include <SDL2/SDL.h>
// 	#include <SDL/SDL_mixer.h>
// }

namespace ncv {
	class AudioReader: public BaseReader {
	public:
		explicit AudioReader(AVStream* stream):
				BaseReader(stream) {}
		
		virtual void process(AVFrame*) override {}
	};

	// const int SDL_AUDIO_BUFFER_SIZE = 1024;
	// const int MAX_AUDIO_FRAME_SIZE = 192000;


	// class AudioReader: public BaseReader {
	// 	class PacketQueue {
	// 		deque<AVPacket*> packets;
	// 		int size = 0;

	// 		SDL_mutex* mutex;
	// 		SDL_cond* cond;

	// 	public:
	// 		PacketQueue():
	// 			mutex(SDL_CreateMutex()),
	// 			cond(SDL_CreateCond()) {}
			
	// 		void push(const AVPacket* packet) {
	// 			SDL_LockMutex(mutex);

	// 			packets.push_back(throwIfNull(av_packet_clone(packet)));
	// 			size += packet->size;

	// 			SDL_CondSignal(cond);
	// 			SDL_UnlockMutex(mutex);
	// 		}

	// 		AVPacket* pop(bool block) {
	// 			SDL_LockMutex(mutex);

	// 			while (!stopped) {
	// 				if (!packets.empty()) {
	// 					AVPacket* packet = packets.front();
	// 					packets.pop_front();

	// 					size -= packet->size;

	// 					SDL_UnlockMutex(mutex);

	// 					return packet;
	// 				}

	// 				if (block) {
	// 					SDL_CondWait(cond, mutex);
	// 				}
	// 			}

				
	// 			SDL_UnlockMutex(mutex);

	// 			return nullptr;
	// 		}
	// 	};


	// 	SDL_AudioSpec wantedSpec, spec;
	// 	PacketQueue packetQueue;


	// public:
	// 	AudioReader(AVStream* stream):
	// 			BaseReader(stream),
	// 			wantedSpec {
	// 				.freq = codecContext->sample_rate,
	// 				.format = AUDIO_S16SYS,
	// 				.channels = static_cast<Uint8>(codecContext->channels),
	// 				.silence = 0,
	// 				.samples = SDL_AUDIO_BUFFER_SIZE,
	// 				.padding = 0, // ?
	// 				.userdata = codecContext
	// 			} {}
		
	// 	virtual void process(const AVPacket* packet, AVFrame*) override {
	// 		packetQueue.push(packet);
	// 	}
	
	// private:
	// 	void callback(void* userdata, uint8_t* stream, int len) {
	// 		static uint8_t buffer[MAX_AUDIO_FRAME_SIZE * 3 / 2];

	// 		static int buffer_size = 0;
	// 		static int buffer_index = 0;

	// 		while (len > 0) {

	// 			if (buffer_index >= buffer_size) {

	// 				// We have already sent all our data; get more
	// 				int audio_size = audio_decode_frame(codecContext, buffer, sizeof(buffer));

	// 				if (audio_size < 0) {
	// 					// If error, output silence
	// 					buffer_size = SDL_AUDIO_BUFFER_SIZE;
	// 					memset(buffer, 0, buffer_size);

	// 				} else {
	// 					buffer_size = audio_size;
	// 				}

	// 				buffer_index = 0;
	// 			}

	// 			int diff = min(buffer_size - buffer_index, len);

	// 			memcpy(stream, buffer + buffer_index, diff);

	// 			len -= diff;
	// 			stream += diff;
	// 			buffer_index += diff;
	// 		}
	// 	}


	// 	int audio_decode_frame(AVCodecContext *codecContext, uint8_t* buffer, int buf_size) {

	// 		AVPacket pkt;
	// 		uint8_t* audio_pkt_data = nullptr;
	// 		int audio_pkt_size = 0;
	// 		AVFrame frame;

	// 		for(;;) {
	// 			while (audio_pkt_size > 0) {
	// 				int got_frame = 0;

	// 				int diff = avcodec_decode_audio4(codecContext, &frame, &got_frame, &pkt);

	// 				if(diff < 0) {
	// 					/* if error, skip frame */
	// 					audio_pkt_size = 0;
	// 					break;
	// 				}
					
	// 				audio_pkt_data += diff;
	// 				audio_pkt_size -= diff;

	// 				int data_size = 0;

	// 				if (got_frame) {
	// 					data_size = av_samples_get_buffer_size(
	// 						nullptr, codecContext->channels, frame.nb_samples,
	// 						codecContext->sample_fmt, 1
	// 					);

	// 					assert(data_size <= buf_size);

	// 					memcpy(buffer, frame.data[0], data_size);
	// 				}

	// 				if (data_size <= 0) {
	// 					/* No data yet, get more frames */
	// 					continue;
	// 				}

	// 				/* We have data, return it and come back for more later */
	// 				return data_size;
	// 			}

	// 			if (pkt.data != nullptr) {
	// 				av_packet_unref(&pkt);
	// 			}

	// 			if (stopped) {
	// 				return -1;
	// 			}

	// 			if (packetQueue.pop(true) == nullptr) {
	// 				return -1;
	// 			}

	// 			audio_pkt_data = pkt.data;
	// 			audio_pkt_size = pkt.size;
	// 		}
	// 	}
	// };
}
