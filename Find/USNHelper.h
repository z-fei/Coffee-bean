#pragma once
#include <vector>
#include <Winioctl.h>
#include <map>
#include "FileInfo.h"
// CUSNHelper

class CUSNHelper
{
public:
	CUSNHelper();
	virtual ~CUSNHelper();

	BOOL GetAllFiles(CDiskInfo& disk);
	CString GetFilePath(CUSNFileInfo& fileInfo, CDiskInfo& disk);

	std::vector<CDiskInfo> m_diskList;
private:
	void InitNTFSDrive();
	BOOL IsNTFS(CString vol);

	BOOL USNOperate(CDiskInfo& disk);
	BOOL OpenDriveIndex(CDiskInfo& disk);
	BOOL CreateUSNJournal(CDiskInfo& disk);
	BOOL QueryUSNJournal(CDiskInfo& disk);
	BOOL GetUSNJournalFiles(CDiskInfo& disk);
	BOOL DeleteUSNJournal(CDiskInfo& disk);
};


