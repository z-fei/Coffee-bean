// FileManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FileManager.h"
#include <thread>

// FileManager

CFileManager::CFileManager(std::mutex* pMutex) :
    m_pMutex(pMutex)
{
}

CFileManager::~CFileManager()
{
}

ULONGLONG CFileManager::GetAllFileMapSize()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    return m_allFileMap.size();
}

void CFileManager::InsertToAllFileMap(CString fileName, CFileInfo fileInfo)
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    m_allFileMap.insert(std::make_pair(fileName, fileInfo));
}

BOOL CFileManager::GetAllFileMapEqualRange(CString searchName, 
    std::pair<std::multimap<CString, CFileInfo>::iterator, std::multimap<CString, CFileInfo>::iterator>& itEqual)
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    auto it = m_allFileMap.equal_range(searchName);
    if (it.first == m_allFileMap.end())
        return FALSE;

    itEqual = it;
    return TRUE;
}

std::vector<CDisplayFileInfo> CFileManager::GetFindFiles()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    return m_findFiles;
}

void CFileManager::PushFindFiles(CDisplayFileInfo fileInfo)
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    m_findFiles.push_back(fileInfo);
}

void CFileManager::RelaseFindFiles()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    std::vector<CDisplayFileInfo> temp;
    m_findFiles.clear();
    m_findFiles.swap(temp);
}

std::vector<CDisplayFileInfo> CFileManager::GetFolderFiles()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    return m_folderFiles;
}

void CFileManager::PushFolderFiles(CDisplayFileInfo fileInfo)
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    m_folderFiles.push_back(fileInfo);
}

void CFileManager::RelaseFolderFiles()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    std::vector<CDisplayFileInfo> temp;
    m_folderFiles.clear();
    m_folderFiles.swap(temp);
}

CString CFileManager::GetFolderFromQueue()
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    CString path = m_folderQueue.front();
    m_folderQueue.pop();

    return path;
}

void CFileManager::PushFolderToQueue(CString folder)
{
    std::unique_lock<std::mutex> lock{ *m_pMutex };
    m_folderQueue.push((LPCWSTR)folder);
}



