#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
	ThreadPool(int nThreads);
	~ThreadPool();

	template<typename T>
	auto AddSimpleTask(T&& function)
	{
		{
			std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);
			m_taskQueue.emplace(std::forward<T>(function));
		}
		m_poolNotifier.notify_one();
	}


	template<typename T, typename...Args>
	auto AddTask(T&& function, Args && ...args)
		-> std::future<typename std::invoke_result_t<T, Args...>>
	{
		using return_t = typename std::invoke_result_t<T, Args...>;

		auto task = std::make_shared<std::packaged_task<return_t()>>(
			std::bind(std::forward<T>(function), std::forward<Args>(args)...));
		
		std::future<return_t> result = task->get_future();
		
		{
			std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);

			m_taskQueue.emplace([task]() { (*task)(); });
		}
		
		m_poolNotifier.notify_one();

		return result;
	}

	void EmergencyStop();
	void Pause(bool pauseState);
	bool IsEmpty() { return m_taskQueue.empty(); }

private:
	void Worker();

	std::vector<std::thread> m_threads{};
	std::queue<std::function<void()>> m_taskQueue{};
	std::recursive_mutex m_queueMutex{};
	std::condition_variable_any m_poolNotifier{};
	bool m_stopProcessing;
	bool m_emergencyStop;
	bool m_paused;
};