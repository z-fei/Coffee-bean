// FunctionManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FunctionManager.h"


// CFunctionManager
CFunctionManager::CFunctionManager() :
    m_threadManager(8, this),
    m_pView(nullptr)
{
}

CFunctionManager::~CFunctionManager()
{
}

void CFunctionManager::AddFunction(SEARCH_EVENT eventName, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)> func)
{
    m_eventFuncMap.insert(make_pair(eventName, func));
}

void CFunctionManager::ExecFunc(SEARCH_EVENT eventName, LPVOID lp, INT progress, CString folderName)
{
    {
        switch (eventName)
        {
        case INIT_DISK_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, 0);
            break;
        case USN_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, 0);
            break;
        case FOLDER_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_folderMap[folderName], 0);
            break;
        case UPDATE_PROGRESS:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, progress);
            break;
        default:
            break;
        }

    }
}

void CFunctionManager::DoSearchUSN(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };
        functionManager->m_fileManager.m_findFiles.clear();
        INT index = 0;
        for (std::vector<CDiskInfo>::iterator it = functionManager->m_fileManager.m_USNHelper.m_diskList.begin(); it < functionManager->m_fileManager.m_USNHelper.m_diskList.end(); ++it)
        {
            for (std::vector<CUSNFileInfo>::iterator fileIt = it->m_allDriveFiles.begin(); fileIt < it->m_allDriveFiles.end(); fileIt++)
            {
                if (fileIt->m_fileName.Compare(functionManager->m_searchName) == 0)
                {
                    CFileStatus fileStatus;
                    CString fileSize;
                    CString filePath = fileIt->m_rootPath + functionManager->m_fileManager.m_USNHelper.GetFilePath(*fileIt, *it) + fileIt->m_fileName;

                    //struct _stat info;
                    //_stat((char*)(LPCTSTR)filePath, &info);
                    //int size = info.st_size;
                    //fileSize.Format(_T("%d%s"), size, _T("KB"));
                    if (CFile::GetStatus(filePath, fileStatus))
                    {
                        fileSize.Format(_T("%I64u"), fileStatus.m_size);
                        fileSize += _T("KB");
                    }
                    CDisplayFileInfo fileInfo(fileIt->m_fileName, fileIt->m_isFolder, fileSize, filePath);
                    functionManager->m_fileManager.m_findFiles.push_back(fileInfo);
                    if (fileInfo.m_isFolder)
                    {
                        functionManager->m_fileManager.m_folders.push(fileInfo);
                        functionManager->m_fileManager.m_folderMap.insert(std::make_pair(fileInfo.m_filePath, 0));
                    }
                }
            }
            INT progress = ++index * 100 / functionManager->m_fileManager.m_USNHelper.m_diskList.size();
            functionManager->ExecFunc(SEARCH_EVENT(UPDATE_PROGRESS), functionManager->m_pView, progress);
        }

        functionManager->ExecFunc(SEARCH_EVENT(USN_SEARCH_COMPLETE), functionManager->m_pView, 0);
        if (!functionManager->m_fileManager.m_folders.empty())
        {
            functionManager->m_threadManager.SubmitTask(DoSearchFolder);
        }
    }
}

void CFunctionManager::DoSearchFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };
        for (int i = 0; i < functionManager->m_fileManager.m_folders.size(); i++)
        {
            functionManager->m_threadManager.SubmitTask(DoRraverseFolder);
        }
    }
}

void CFunctionManager::DoRraverseFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };
        INT progress = 100 / functionManager->m_fileManager.m_folders.size();
        CString fullName = functionManager->m_fileManager.m_folders.front().m_filePath;
        functionManager->m_fileManager.m_folders.pop();
        functionManager->m_fileManager.Recurse(fullName, fullName);

        functionManager->ExecFunc(SEARCH_EVENT(UPDATE_PROGRESS), functionManager->m_pView, progress);

        if (functionManager->m_fileManager.m_folders.empty())
        {
            functionManager->ExecFunc(SEARCH_EVENT(FOLDER_SEARCH_COMPLETE), functionManager->m_pView, 0, fullName);
        }
    }
}

void CFunctionManager::Initialize(LPVOID lp)
{
    m_pView = lp;
    m_fileManager.Initialize(&m_mutex);
}

void CFunctionManager::DoSearch(CString searchName)
{
    m_searchName = searchName;
    m_threadManager.SubmitTask(DoSearchUSN);
}

void CFunctionManager::InitDisk()
{
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        int index = 0;
        for (std::vector<CDiskInfo>::iterator it = m_fileManager.m_USNHelper.m_diskList.begin(); it != m_fileManager.m_USNHelper.m_diskList.end(); it++)
        {
            it->m_allDriveFiles.clear();
            it->m_parentFrnMap.clear();
            m_fileManager.m_USNHelper.GetAllFiles(*it);
            INT progress = ++index * 100 / m_fileManager.m_USNHelper.m_diskList.size();
            ExecFunc(SEARCH_EVENT(UPDATE_PROGRESS), m_pView, progress);
        }

        ExecFunc(SEARCH_EVENT(INIT_DISK_COMPLETE), m_pView, 0);
    }

}

std::vector<CDisplayFileInfo> CFunctionManager::GetFolderSubFiles(CString& filePath)
{
    std::unique_lock<std::mutex> { m_mutex };
    return m_fileManager.m_folderMap[filePath];
}

// CFunctionManager message handlers


