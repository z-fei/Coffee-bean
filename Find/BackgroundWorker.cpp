// BackgroundWorker.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "BackgroundWorker.h"

// CBackgroundWorker
CBackgroundWorker::CBackgroundWorker()
{
}

CBackgroundWorker::~CBackgroundWorker()
{

}

void CBackgroundWorker::Initialize(CFunctionManager* functionManager)
{
	m_functionManager = functionManager;
	m_hThread = (HANDLE)AfxBeginThread(TreadFunction, (LPVOID)functionManager);
}

UINT CBackgroundWorker::TreadFunction(LPVOID lp)
{
	auto functionManager = (CFunctionManager*)lp;
	functionManager->InitDisk();

	return 0;
}



// CBackgroundWorker message handlers


