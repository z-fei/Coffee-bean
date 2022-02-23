// ThreadManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "ThreadManager.h"

unsigned int Hardware_ConCurrency() 
{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

// CThreadManager
CThreadManager::CThreadManager(LPVOID lp) :
	m_lp(lp)
{
	InitThreadPool(Hardware_ConCurrency() * 2);
}

CThreadManager::~CThreadManager()
{
	m_running = false;
	m_taskCV.notify_all();

    for (std::thread& thread : m_threads)
	{
		if (thread.joinable())
			thread.join();
	}
}

void CThreadManager::SubmitTask(std::function<void(LPVOID)> task)
{
	if (!m_running) return;

	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_tasks.emplace(task);
	}

	m_taskCV.notify_one();
}

void CThreadManager::InitThreadPool(UINT threadNum)
{
	for (UINT i = 0; i < threadNum; ++i)
	{
		m_threads.emplace_back([this] { 
			while (m_running)
			{
				std::function<void(LPVOID)> task;
				{
					std::unique_lock<std::mutex> lock{ m_mutex };
					m_taskCV.wait(lock, [this] {return !m_running || !m_tasks.empty();});

					if (!m_running && m_tasks.empty())
						return;

					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				m_idleThreadNum--;
				task(m_lp);
				m_idleThreadNum++;
			}
			});
		m_idleThreadNum++;
	}
}


