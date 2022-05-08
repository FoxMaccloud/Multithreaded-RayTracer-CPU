#include "ThreadPool.h"

ThreadPool::ThreadPool(int nThreads)
{
	m_stopProcessing = false;
	m_emergencyStop = false;
	m_paused = false;

	if (nThreads == 0)
		throw std::runtime_error("ERROR: Threadpool must have atleats 1 thread!");
	m_threads.reserve(nThreads);

	for (int i = 0; i < nThreads; i++)
	{
		m_threads.emplace_back([this]() {worker(); });
	}
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);
	m_stopProcessing = true;
	m_poolNotifier.notify_all();
	for (auto& taskThread : m_threads)
	{
		if (taskThread.joinable())
			taskThread.join();
	}
}

void ThreadPool::worker()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);
			m_poolNotifier.wait(queueLock, [this]()
				{
					return (!m_taskQueue.empty() && !m_paused) || m_stopProcessing || m_emergencyStop;
				});
			if ((m_stopProcessing && m_taskQueue.empty()) || m_emergencyStop)
				return;
			
			task = std::move(m_taskQueue.front());
			m_taskQueue.pop();
		}
		task();
	}
}

void ThreadPool::emergency_stop()
{
	std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);
	m_emergencyStop = true;
	m_poolNotifier.notify_all();
}

void ThreadPool::pause(bool pauseState)
{
	std::unique_lock<std::recursive_mutex> queueLock(m_queueMutex);
	m_paused = pauseState;
	m_poolNotifier.notify_all();
}