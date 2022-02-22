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

// CFunctionManager
class CFunctionManager
{
public:
	CFunctionManager(LPVOID lp);
	virtual ~CFunctionManager();

public:
	//Event handler
	void AddFunction(SEARCH_EVENT eventName, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)> func);
	void ExecFunc(SEARCH_EVENT eventName, LPVOID lp, INT progress);

	//Action
	BOOL OnInitDisk();
	BOOL OnSearch(CString name);
	BOOL OnSearchFolder(CString& filePath);
private:
	//Task
	static void DoSearchAllFileMap(LPVOID lp);
	static void DoSearchFolder(LPVOID lp);
	static void DoRraverseFolder(LPVOID lp);

	void Recurse(CString path);
	void RecurseFolder(CString path);

	ULONGLONG GetFileSize(CString path) const;

	ULONGLONG GetTotalSize();
	void SetTotalSize(ULONGLONG size);
	void ResetTotalSize();

	void AddTaskCount();
	void SubTaskCount();
	void ResetTaskCount();
	ULONGLONG GetTaskCount();

	void AddProgressCount();
	void SubProgressCount();
	void ResetProgressCount();
	ULONGLONG GetProgressCount();

private:
	LPVOID m_pView;
	CFileManager m_fileManager;
	CThreadManager m_threadManager;
	
	UINT m_progressCount;
	UINT m_taskCount;
	ULONGLONG m_totalSize;

	CString m_searchName;
	CString m_searchFolderName;

	std::mutex m_mutex;
	std::map<SEARCH_EVENT, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)>> m_eventFuncMap;
};


