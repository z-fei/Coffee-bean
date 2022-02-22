// FunctionManager.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FunctionManager.h"


// CFunctionManager
CFunctionManager::CFunctionManager(LPVOID lp) :
	m_pView(lp),
	m_fileManager(&m_mutex),
	m_threadManager(this),
	m_progressCount(0),
	m_taskCount(0),
	m_totalSize(0)
{
}

CFunctionManager::~CFunctionManager()
{
}

void CFunctionManager::AddTaskCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_taskCount++;
}

ULONGLONG CFunctionManager::GetTotalSize()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    return m_totalSize;
}

void CFunctionManager::SetTotalSize(ULONGLONG size)
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_totalSize += size;
}

void CFunctionManager::ResetTotalSize()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_totalSize = 0;
}

ULONGLONG CFunctionManager::GetFileSize(CString path) const
{
    LONGLONG size = 0;
    if (path.IsEmpty() || path.GetLength() > 256)
        return size;

    CFileStatus status;
    if (CFile::GetStatus((LPCTSTR)path, status))
            size = status.m_size;

    return size;
}

void CFunctionManager::SubTaskCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_taskCount--;
}

void CFunctionManager::ResetTaskCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_taskCount = 0;
}

ULONGLONG CFunctionManager::GetTaskCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    return m_taskCount;
}

void CFunctionManager::ResetProgressCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_progressCount = 0;
}

void CFunctionManager::AddProgressCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_progressCount++;
}

void CFunctionManager::SubProgressCount()
{
    std::unique_lock<std::mutex> lock{ m_mutex };
    m_progressCount--;
}

ULONGLONG CFunctionManager::GetProgressCount()
{
    ULONGLONG count = m_fileManager.GetAllFileMapSize();

    std::unique_lock<std::mutex> lock{ m_mutex };
    if (count < 300000)
    {
        if (m_progressCount < 30000)
            m_progressCount += 1;
    }
    else if (count < 600000)
    {
        if (m_progressCount < 60000)
            m_progressCount += 1;
    }
    else
    {
        if (m_progressCount < 90000)
            m_progressCount += 1;
    }
    return m_progressCount;
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
            m_fileManager.InsertToAllFileMap(find.GetFileName(), fileInfo);

            if (find.IsDirectory())
            {
                if (m_threadManager.m_idleThreadNum > 0)
                {
                    m_fileManager.PushFolderToQueue(find.GetFilePath());
                    m_threadManager.SubmitTask(DoRraverseFolder);
                    AddTaskCount();
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
            CString size;
            ULONGLONG uSize = GetFileSize(find.GetFilePath());
            size.Format(_T("%I64u %s"), uSize, _T(" bytes"));
            CDisplayFileInfo fileInfo(find.GetFileName(), find.IsDirectory(), size, find.GetFilePath());
            m_fileManager.PushFolderFiles(fileInfo);
            SetTotalSize(uSize);

            if (find.IsDirectory())
            {
                if (m_threadManager.m_idleThreadNum > 0)
                {
                    m_fileManager.PushFolderToQueue(find.GetFilePath());
                    m_threadManager.SubmitTask(DoSearchFolder);
                    AddTaskCount();
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
            m_eventFuncMap[eventName](lp, m_fileManager.GetFindFiles(), progress);
            break;
        case FILE_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.GetFindFiles(), progress);
            break;
        case FOLDER_SEARCH_COMPLETE:
            m_eventFuncMap[eventName](lp, m_fileManager.GetFolderFiles(), progress);
            break;
        case UPDATE_PROGRESS:
            m_eventFuncMap[eventName](lp, m_fileManager.GetFindFiles(), progress);
            break;
        default:
            break;
        }
    }
}

void CFunctionManager::DoSearchAllFileMap(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;
    std::pair<std::multimap<CString, CFileInfo>::iterator, std::multimap<CString, CFileInfo>::iterator> itMap;
    if (functionManager->m_fileManager.GetAllFileMapEqualRange(functionManager->m_searchName, itMap))
    {
        for (auto it = itMap.first; it != itMap.second; it++)
        {
            CString size;
            size.Format(_T("%I64u %s"), functionManager->GetFileSize(it->second.m_filePath), _T("bytes"));
            CDisplayFileInfo fileInfo(it->second.m_fileName, it->second.m_isFolder, size, it->second.m_filePath);
            functionManager->m_fileManager.PushFindFiles(fileInfo);
        }
    }

    functionManager->ExecFunc(SEARCH_EVENT(FILE_SEARCH_COMPLETE), functionManager->m_pView, 100);
    functionManager->m_fileManager.RelaseFindFiles();
}

void CFunctionManager::DoSearchFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;

    CString path = functionManager->m_fileManager.GetFolderFromQueue();
    functionManager->RecurseFolder(path);
    functionManager->SubTaskCount();

    if (functionManager->GetTaskCount() == 0)
    {
        CString size;
        size.Format(_T("%I64u %s"), functionManager->GetTotalSize(), _T(" bytes"));
        CDisplayFileInfo fileInfo(_T(""), TRUE, size, _T(""));
        functionManager->m_fileManager.PushFolderFiles(fileInfo);

        functionManager->ExecFunc(SEARCH_EVENT(FOLDER_SEARCH_COMPLETE), functionManager->m_pView, 0);
        functionManager->ResetTaskCount();
    }
}

void CFunctionManager::DoRraverseFolder(LPVOID lp)
{
    auto functionManager = (CFunctionManager*)lp;

    CString path = functionManager->m_fileManager.GetFolderFromQueue();
    functionManager->Recurse(path);

    ULONGLONG progress = functionManager->GetProgressCount();
    functionManager->ExecFunc(SEARCH_EVENT(UPDATE_PROGRESS), functionManager->m_pView, progress);
    functionManager->SubTaskCount();

    if (functionManager->GetTaskCount() == 0)
    {
        functionManager->ExecFunc(SEARCH_EVENT(INIT_DISK_COMPLETE), functionManager->m_pView, 100);
        functionManager->ResetTaskCount();
    }
}

BOOL CFunctionManager::OnInitDisk()
{
    for (wchar_t d = 'A'; d < 'Z'; d++)
    {
        TCHAR temp[3] = {d, ':', '\0'};
        UINT type = GetDriveType(temp);
        if (type == DRIVE_FIXED)
        {
            m_fileManager.m_diskList.push_back((LPCWSTR)temp);
            m_fileManager.PushFolderToQueue((LPCWSTR)temp);
        }
    }

    for (std::vector<CString>::iterator it = m_fileManager.m_diskList.begin(); it != m_fileManager.m_diskList.end(); it++)
    {
        m_threadManager.SubmitTask(DoRraverseFolder);
        AddTaskCount();
    }

    return TRUE;
}

BOOL CFunctionManager::OnSearch(CString searchName)
{
    if (GetTaskCount() != 0)
        return FALSE;

    m_searchName = searchName;
    m_threadManager.SubmitTask(DoSearchAllFileMap);

    return TRUE;
}

BOOL CFunctionManager::OnSearchFolder(CString& filePath)
{
    if (m_searchFolderName.Compare(filePath) == 0)
    {
        ExecFunc(SEARCH_EVENT(FOLDER_SEARCH_COMPLETE), m_pView, 0);
        return TRUE;
    }

    m_searchFolderName = filePath;
    m_fileManager.RelaseFolderFiles();
    ResetTotalSize();
    m_fileManager.PushFolderToQueue(m_searchFolderName);
    m_threadManager.SubmitTask(DoSearchFolder);
    AddTaskCount();

    return TRUE;
}


