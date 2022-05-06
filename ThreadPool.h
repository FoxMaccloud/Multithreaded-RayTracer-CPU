#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool
{
public:
	ThreadPool(int nThreads);
	~ThreadPool();

	template <typename T>
	auto add_simple_task(T&& function);

	template<typename T, typename... Args>
	std::future<std::invoke_result_t<T, Args...>> add_task(T&& function, Args &&...args);

	void emergency_stop();
	void pause(bool pauseState);
	bool is_empty() { return m_taskQueue.empty(); }

private:
	void worker();

	std::vector<std::thread> m_threads{};
	std::queue<std::function<void()>> m_taskQueue{};
	std::recursive_mutex m_queueMutex{};
	std::condition_variable_any m_poolNotifier{};
	bool m_stopProcessing;
	bool m_emergencyStop;
	bool m_paused;
};