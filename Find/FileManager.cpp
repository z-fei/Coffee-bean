// FileManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FileManager.h"
#include <thread>

// FileManager

CFileManager::CFileManager()
{

}

CFileManager::~CFileManager()
{
}

void CFileManager::Initialize(std::mutex* mutex)
{
	m_pMutex = mutex;
}

void CFileManager::Recurse(CString folderKey, CString filePath)
{
    CFileFind find;
    CString fullPath = filePath + _T("\\\*.*");
    BOOL isFind = find.FindFile(fullPath);

    CFileStatus fileStatus;
    while (isFind)
    {
        isFind = find.FindNextFile();
        if (!find.IsDots())
        {
            CString fileSize;
            if (CFile::GetStatus(find.GetFilePath(), fileStatus))
            {
                fileSize.Format(_T("%I64u"), fileStatus.m_size);
                fileSize += _T("KB");
            }

            CDisplayFileInfo fileInfo(find.GetFileName(), find.IsDirectory(), fileSize, find.GetFilePath());
            m_folderMap[folderKey].push_back(fileInfo);

            if (find.IsDirectory())
            {
                Recurse(folderKey, find.GetFilePath());
            }
        }
        else
        {
            continue;
        }
    }
}


