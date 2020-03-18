#pragma once
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <future>

class threadPool {
public:
	using Task = std::function<void()>;
	explicit threadPool(size_t threadCount) {
		start(threadCount);
	}
	~threadPool() {
		stop();
	}
	void enqueue(Task task)
	{
		{
			std::unique_lock<std::mutex> lock{ nEventMutex };
			nTasks.emplace(std::move(task));
		}
		nEventVar.notify_one();

	}
private:
	std::vector<std::thread> nThreads;
	std::condition_variable nEventVar;
	std::mutex nEventMutex;
	bool nStopping = false;
	std::queue<Task> nTasks;

	void start(size_t threadCount) {
		for (auto i = 0u; i < threadCount; ++i) {
			nThreads.emplace_back([=] {
				while (true) {
					Task task;
					{
						std::unique_lock<std::mutex> lock{ nEventMutex };
						nEventVar.wait(lock, [=] {return nStopping || !nTasks.empty(); });

						if (nStopping && nTasks.empty())
							break;

						task = std::move(nTasks.front());
						nTasks.pop();
					}
					task();
				}
				});
		}
	}
	void stop() noexcept {
		{
			std::unique_lock<std::mutex> lock{ nEventMutex };
			nStopping = true;
		}
		nEventVar.notify_all();
		for (auto& thread : nThreads)
			thread.join();
	}
};