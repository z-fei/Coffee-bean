// FunctionManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FunctionManager.h"


// CFunctionManager
CFunctionManager::CFunctionManager() :
    m_threadManager(this),
    m_pView(nullptr),
    m_count(0)
{
}

CFunctionManager::~CFunctionManager()
{
}

void CFunctionManager::Initialize(LPVOID lp)
{
    m_pView = lp;
}

void CFunctionManager::Recurse(CString path)
{
    CFileFind find;
    BOOL isFind = find.FindFile(path + _T("\\\*.*"));

    while (isFind)
    {
        isFind = find.FindNextFile();
        if (!find.IsDots())
        {
            CFileInfo fileInfo(find.GetFileName(), find.GetFilePath(), find.IsDirectory());
            m_fileManager.m_allFileMap.insert(std::make_pair(find.GetFileName(), fileInfo));

            if (find.IsDirectory())
            {
                if (m_threadManager.m_idleThreadNum > 0)
                {
                    m_fileManager.m_folderQueue.push(find.GetFilePath());
                    m_threadManager.SubmitTask(DoRraverseFolder);
                }
                else
                {
                    Recurse(find.GetFilePath());
                }
            }
            else
            {
                Recurse(find.GetFilePath());
            }
        }
    }
}

void CFunctionManager::RecurseFolder(CString path)
{
    CFileFind find;
    BOOL isFind = find.FindFile(path + _T("\\\*.*"));

    while (isFind)
    {
        isFind = find.FindNextFile();
        if (!find.IsDots())
        {
            CDisplayFileInfo fileInfo(find.GetFileName(), find.IsDirectory(), NULL, find.GetFilePath());
            m_fileManager.m_folderFiles.push_back(fileInfo);

            if (find.IsDirectory())
            {
                if (m_threadManager.m_idleThreadNum > 0)
                {
                    m_fileManager.m_folderQueue.push(find.GetFilePath());
                    m_threadManager.SubmitTask(DoSearchFolder);
                }
                else
                {
                    RecurseFolder(find.GetFilePath());
                }
            }
            else
            {
                RecurseFolder(find.GetFilePath());
            }
        }
    }
}

void CFunctionManager::AddFunction(SEARCH_EVENT eventName, std::function<void(LPVOID, std::vector<CDisplayFileInfo>, INT)> func)
{
    m_eventFuncMap.insert(make_pair(eventName, func));
}

void CFunctionManager::ExecFunc(SEARCH_EVENT eventName, LPVOID lp, INT progress)
{
    {
        switch (eventName)
        {
        case INIT_DISK_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, progress);
            break;
        case FILE_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, progress);
            break;
        case FOLDER_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.m_folderFiles, progress);
            break;
        case UPDATE_PROGRESS:
            m_eventFuncMap[eventName](lp, m_fileManager.m_findFiles, progress);
            break;
        default:
            break;
        }

    }
}

void CFunctionManager::DoSearchAllFileMap(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };
        functionManager->m_fileManager.m_findFiles.clear();
        auto itMap = functionManager->m_fileManager.m_allFileMap.equal_range(functionManager->m_searchName);
        if (itMap.first != functionManager->m_fileManager.m_allFileMap.end())
        {
            for (auto it = itMap.first; it != itMap.second; it++)
            {
                CDisplayFileInfo fileInfo(it->second.m_fileName, it->second.m_isFolder, NULL, it->second.m_filePath);
                functionManager->m_fileManager.m_findFiles.push_back(fileInfo);
            }
        }

        functionManager->ExecFunc(SEARCH_EVENT(FILE_SEARCH_COMPLETE), functionManager->m_pView, 100);
    }
}

void CFunctionManager::DoSearchFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };

        CString path = functionManager->m_fileManager.m_folderQueue.front();
        functionManager->m_fileManager.m_folderQueue.pop();
        functionManager->RecurseFolder(path);

        if (functionManager->m_fileManager.m_folderQueue.empty())
        {
            functionManager->ExecFunc(SEARCH_EVENT(FOLDER_SEARCH_COMPLETE), functionManager->m_pView, 0);
        }
    }
}

void CFunctionManager::DoRraverseFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    {
        std::unique_lock<std::mutex> lock{ functionManager->m_mutex };

        CString path = functionManager->m_fileManager.m_folderQueue.front();
        functionManager->m_fileManager.m_folderQueue.pop();
        functionManager->Recurse(path);

        if (functionManager->m_fileManager.m_allFileMap.size() < 50000)
        {
            if (functionManager->m_count < 30)
                functionManager->m_count += 1;
        }
        else if (functionManager->m_fileManager.m_allFileMap.size() < 100000)
        {
            if (functionManager->m_count < 60)
                functionManager->m_count += 1;
        }
        else
        {
            if (functionManager->m_count < 90)
                functionManager->m_count += 1;
        }
        functionManager->ExecFunc(SEARCH_EVENT(UPDATE_PROGRESS), functionManager->m_pView, functionManager->m_count);

        if (functionManager->m_fileManager.m_folderQueue.empty())
        {
            functionManager->ExecFunc(SEARCH_EVENT(INIT_DISK_COMPLETE), functionManager->m_pView, 100);
        }
    }
}

void CFunctionManager::OnInitDisk()
{
    for (wchar_t d = 'A'; d < 'Z'; d++)
    {
        TCHAR temp[3] = {d, ':', '\0'};
        UINT type = GetDriveType(temp);
        if (type == DRIVE_FIXED || type == DRIVE_REMOTE)
        {
            m_fileManager.m_diskList.push_back((LPCWSTR)temp);
            m_fileManager.m_folderQueue.push((LPCWSTR)temp);
        }
    }

    for (std::vector<CString>::iterator it = m_fileManager.m_diskList.begin(); it != m_fileManager.m_diskList.end(); it++)
    {
        m_threadManager.SubmitTask(DoRraverseFolder);
    }
}

void CFunctionManager::OnSearch(CString searchName)
{
    m_searchName = searchName;
    m_threadManager.SubmitTask(DoSearchAllFileMap);
}

void CFunctionManager::OnSearchFolder(CString& filePath)
{
    m_searchFolderName = filePath;

    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        m_fileManager.m_folderQueue.push(m_searchFolderName);
    }

    m_threadManager.SubmitTask(DoSearchFolder);
}


