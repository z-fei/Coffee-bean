#pragma once
#include <map>
#include <vector>
#include "FileInfo.h"
#include <winioctl.h>


// CDiskInfo
class CDiskInfo
{
public:
	CDiskInfo(CString diskName);
	virtual ~CDiskInfo();

public:
	HANDLE m_hDisk;
	CString m_diskName;
	USN_JOURNAL_DATA m_usnData;

	std::vector<CUSNFileInfo> m_allDriveFiles;
	std::map<DWORDLONG, CUSNFileInfo> m_parentFrnMap;
};


