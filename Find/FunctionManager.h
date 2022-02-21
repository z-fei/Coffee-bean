#pragma once
#include <functional>
#include <map>
#include "FileManager.h"
#include "ThreadManager.h"


typedef enum SEARCH_EVENT
{
	INIT_DISK_COMPLETE = 1,
	FILE_SEARCH_COMPLETE,
	FOLDER_SEARCH_COMPLETE,
	UPDATE_PROGRESS
}SEARCH_EVENT;

typedef enum SEARCH_TASK
{
	SEARCH_FOLDER = 1,
}SEARCH_TASK;

// CFunctionManager
class CFunctionManager
{
public:
	CFunctionManager();
	virtual ~CFunctionManager();

public:
	//Event handler
	void AddFunction(SEARCH_EVENT eventName, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)> func);
	void ExecFunc(SEARCH_EVENT eventName, LPVOID lp, INT progress);


	void Initialize(LPVOID lp);

	//Action
	void OnInitDisk();
	void OnSearch(CString name);
	void OnSearchFolder(CString& filePath);
private:
	//Task
	static void DoSearchAllFileMap(LPVOID lp);
	static void DoSearchFolder(LPVOID lp);
	static void DoRraverseFolder(LPVOID lp);

	void Recurse(CString path);
	void RecurseFolder(CString path);

private:
	LPVOID m_pView;
	CFileManager m_fileManager;
	CThreadManager m_threadManager;
	
	INT m_count;
	CString m_searchName;
	CString m_searchFolderName;

	std::mutex m_mutex;
	std::map<SEARCH_EVENT, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)>> m_eventFuncMap;
	std::map<SEARCH_TASK, std::function<void(LPVOID, std::vector<CDisplayFileInfo>)>> m_taskFuncMap;
};


