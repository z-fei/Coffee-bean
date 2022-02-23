#pragma once

#include "FileInfo.h"
#include <vector>
#include <mutex>
#include <map>
#include <queue>

// FileManager

class CFileManager
{
public:
	CFileManager(std::mutex* pMutex);
	virtual ~CFileManager();

	//Share memory
	ULONGLONG GetAllFileMapSize();
	void InsertToAllFileMap(CString fileName, CFileInfo fileInfo);
	BOOL GetAllFileMapEqualRange(CString searchName, 
		std::pair<std::multimap<CString, CFileInfo>::iterator, std::multimap<CString, CFileInfo>::iterator>& itEqual);

	std::vector<CDisplayFileInfo> GetFindFiles();
	void PushFindFiles(CDisplayFileInfo fileInfo);
	void RelaseFindFiles();

	std::vector<CDisplayFileInfo> GetFolderFiles();
	void PushFolderFiles(CDisplayFileInfo fileInfo);
	void RelaseFolderFiles();

	CString GetFolderFromQueue();
	void PushFolderToQueue(CString folder);

	//Init once
	std::vector<CString> m_diskList;

private:
	std::mutex* m_pMutex;

	std::queue<CString> m_folderQueue;
	std::vector<CDisplayFileInfo> m_findFiles;
	std::vector<CDisplayFileInfo> m_folderFiles;
	std::multimap<CString, CFileInfo> m_allFileMap;
};


