#pragma once

#include "FileInfo.h"
#include <vector>
#include <mutex>
#include "USNHelper.h"
#include <map>
#include <queue>

// FileManager

class CFileManager
{
public:
	CFileManager();
	virtual ~CFileManager();

	void Initialize(std::mutex* mutex);
	void Recurse(CString folderKey, CString filePath);

public:
	CUSNHelper m_USNHelper;

	std::mutex* m_pMutex;
	std::vector<CDisplayFileInfo> m_allFiles;
	std::vector<CDisplayFileInfo> m_findFiles;
	std::queue<CDisplayFileInfo> m_folders;

	std::map<CString, std::vector<CDisplayFileInfo>> m_folderMap;
};


