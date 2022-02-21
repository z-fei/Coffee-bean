#pragma once
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>


// CThreadManager

class CThreadManager
{
public:
	CThreadManager(LPVOID lp);
	virtual ~CThreadManager();

	void SubmitTask(std::function<void(LPVOID)> task);

	std::atomic<int> m_idleThreadNum{ 0 };

private:
	void InitThreadPool(UINT threadNum);

private:
	LPVOID m_lp;

	std::vector<std::thread> m_threads;
	std::queue<std::function<void(LPVOID)>> m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_taskCV;
	std::atomic<bool> m_running{ true };
};


