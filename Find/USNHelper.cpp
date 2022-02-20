// VolumeUtl.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "USNHelper.h"

#define BUF_LEN 0x10000
// CUSNHelper

CUSNHelper::CUSNHelper()
{
	InitNTFSDrive();
}

CUSNHelper::~CUSNHelper()
{
}

BOOL CUSNHelper::IsNTFS(CString vol)
{
	USES_CONVERSION;

	CString rootPathName = vol + _T(":") + _T("\\");
	char lpVolumeNameBuffer[MAX_PATH];
	DWORD lpVolumeSerialNumber;
	DWORD lpMaximumComponentLength;
	DWORD lpFileSystemFlags;
	char lpFileSystemNameBuffer[MAX_PATH];

	if (GetVolumeInformationA((LPCSTR)T2A(rootPathName), lpVolumeNameBuffer, MAX_PATH, &lpVolumeSerialNumber, &lpMaximumComponentLength, &lpFileSystemFlags, lpFileSystemNameBuffer, MAX_PATH))
	{
		if (!strcmp(lpFileSystemNameBuffer, "NTFS")) 
		{
			return true;
		}
	}
	return false;
}

void CUSNHelper::InitNTFSDrive()
{
	for (auto i = 0; i < 26; ++i)
	{
		CString strVol(char(i + 'A'));
		if (IsNTFS(strVol))
		{
			m_diskList.push_back(CDiskInfo(strVol));
		}
	}
}

 BOOL CUSNHelper::USNOperate(CDiskInfo& disk)
 {
	 if (!OpenDriveIndex(disk)) return FALSE;
	 if (!CreateUSNJournal(disk)) return FALSE;
	 if (!QueryUSNJournal(disk)) return FALSE;
	 if (!GetUSNJournalFiles(disk)) return FALSE;
	 if (!DeleteUSNJournal(disk)) return FALSE;

	 return TRUE;
 }

 BOOL CUSNHelper::OpenDriveIndex(CDiskInfo& disk)
 {
	 //Get volumne handle
	 CString volumneName = _T("\\\\.\\") + disk.m_diskName + _T(":");
	 disk.m_hDisk = CreateFile(volumneName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	 if (INVALID_HANDLE_VALUE == disk.m_hDisk)
		 return FALSE;

	 return TRUE;
 }

 BOOL CUSNHelper::CreateUSNJournal(CDiskInfo& disk)
{
	//Create USN
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;
	cujd.AllocationDelta = 0;
	DWORD br;
	if (!DeviceIoControl(disk.m_hDisk, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL))
		return FALSE;

    return TRUE;
}

 BOOL CUSNHelper::QueryUSNJournal(CDiskInfo& disk)
 {
	 //Query USN
	 DWORD br;
	 if (!DeviceIoControl(disk.m_hDisk, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &disk.m_usnData, sizeof(USN_JOURNAL_DATA), &br, NULL))
		 return FALSE;

	 return TRUE;
 }

BOOL CUSNHelper::GetUSNJournalFiles(CDiskInfo& disk)
{
	MFT_ENUM_DATA mftData;
	mftData.StartFileReferenceNumber = 0;
	mftData.LowUsn = disk.m_usnData.FirstUsn;
	mftData.HighUsn = disk.m_usnData.NextUsn;
	mftData.MaxMajorVersion = 2;
	mftData.MinMajorVersion = 0;

	BYTE pData[sizeof(DWORDLONG) + 0x10000];
	DWORD cb;
	while (0 != DeviceIoControl(disk.m_hDisk, FSCTL_ENUM_USN_DATA, &mftData, sizeof(mftData), pData, sizeof(pData), &cb, NULL))
	{

		PUSN_RECORD pRecord = (PUSN_RECORD)&pData[sizeof(USN)];
		while ((PBYTE)pRecord < (pData + cb))
		{  	
			CString CfileName(pRecord->FileName, pRecord->FileNameLength / sizeof(WCHAR));
			BOOL isFolder = (pRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;

			CUSNFileInfo fileInfo(CfileName, isFolder, pRecord->FileReferenceNumber, pRecord->ParentFileReferenceNumber, disk.m_diskName + _T(":\\"));

			disk.m_allDriveFiles.push_back(fileInfo);
			disk.m_parentFrnMap.insert(std::make_pair(fileInfo.m_fileRefID, fileInfo));

			//Next data
			pRecord = (PUSN_RECORD)((PBYTE)pRecord + pRecord->RecordLength);

		}
		//Next page
		mftData.StartFileReferenceNumber = *(DWORDLONG*)pData;
	}

	return TRUE;
}

BOOL CUSNHelper::DeleteUSNJournal(CDiskInfo& disk)
{
    return TRUE;
}

CString CUSNHelper::GetFilePath(CUSNFileInfo& fileInfo, CDiskInfo& disk)
{
	CString path;
	std::map<DWORDLONG, CUSNFileInfo>::iterator it = disk.m_parentFrnMap.find(fileInfo.m_fileParentRefID);
	if (it != disk.m_parentFrnMap.end())
	{
		if (it->second.m_fileParentRefID != 0)
		{
			path = GetFilePath(it->second, disk);
		}

		path += it->second.m_fileName;
		path += (_T("\\"));
	}

	return path;
}

BOOL CUSNHelper::GetAllFiles(CDiskInfo& disk)
{
	return  USNOperate(disk);
}


