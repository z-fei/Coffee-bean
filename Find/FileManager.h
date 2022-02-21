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
	CFileManager();
	virtual ~CFileManager();

public:
	std::vector<CString> m_diskList;
	std::queue<CString> m_folderQueue;

	std::vector<CDisplayFileInfo> m_findFiles;
	std::vector<CDisplayFileInfo> m_folderFiles;

	std::multimap<CString, CFileInfo> m_allFileMap;
};


