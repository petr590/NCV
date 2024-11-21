#include "drawer.h"
#include "frame_group.h"
#include "interthread.h"
#include "alert.h"
#include <functional>

#include "debug.h"

namespace ncv {
	namespace this_thread = std::this_thread;
	using std::vector;
	using std::mutex;
	using std::lock_guard;
	using std::future;
	using std::future_status;
	using std::function;

	static const std::chrono::milliseconds ZERO(0);

	bool parallel = false;

	int frameGroupsProcessPercent = 0;

	void drawFrameGroup(FrameGroup& group, const File& file) {
		erase();
		group.initColors();
		group.draw(file);
	}


	void drawFirstCycle(
			const File& file, const future<void>& readThread,
			function<void(FrameGroup&&)> onFrameGroupRelease
	) {
		alert(L"Loading...");
		
		if (!parallel) {
			int percent = -1;

			while (readThread.wait_for(ZERO) != future_status::ready) {
				if (percent != frameGroupsProcessPercent) {
					percent = frameGroupsProcessPercent;
					alert(L"Loading... " + std::to_wstring(percent) + L"%");
				}

				this_thread::yield();
			}

			timeout(-1);

			lock_guard<mutex> lock(frameGroupsMutex);

			for (FrameGroup& group : frameGroups) {
				drawFrameGroup(group, file);
				onFrameGroupRelease(std::move(group));

				if (stopped) break;
			}

			frameGroups.clear();

		} else {
			while (!stopped) {
				frameGroupsMutex.lock();

				if (!frameGroups.empty()) {
					FrameGroup group = std::move(frameGroups.front());
					frameGroups.pop_front();

					frameGroupsMutex.unlock();

					drawFrameGroup(group, file);

					onFrameGroupRelease(std::move(group));
				} else {

					frameGroupsMutex.unlock();

					if (readThread.wait_for(ZERO) == future_status::ready) {
						break;
					}
				}

				this_thread::yield();
			}
		}
	}


	void drawFrameGroups(const File& file, const future<void>& readThread) {
		bool isGif = file.path().extension() == ".gif";

		if (isGif) {
			vector<FrameGroup> groups;

			drawFirstCycle(file, readThread, [&] (FrameGroup&& group) {
				groups.emplace_back(std::move(group));
			});

			// Проверяем, что гифка содержит больше 1 кадра
			if (isGif && (groups.size() > 1 || (groups.size() == 1 && groups[0].size() > 1))) {
				while (!stopped) {
					for (FrameGroup& group : groups) {
						drawFrameGroup(group, file);
					}
				}
			}

		} else {
			drawFirstCycle(file, readThread, [] (FrameGroup&&) {});
		}
	}
}