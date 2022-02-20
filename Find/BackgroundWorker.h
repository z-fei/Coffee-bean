#pragma once
#include "FileManager.h"
#include "FunctionManager.h"

// CBackgroundWorker

class CBackgroundWorker
{
public:
	CBackgroundWorker();
	virtual ~CBackgroundWorker();

	void Initialize(CFunctionManager* functionManager);

private:
	HANDLE m_hThread;
	CFunctionManager* m_functionManager;

	static UINT TreadFunction(LPVOID lp);
};


