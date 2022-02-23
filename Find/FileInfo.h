#pragma once

// FileInfo
class CFileInfo
{
public:
	CFileInfo();
	CFileInfo(CString fileName, CString filePath, BOOL isFolder);
	virtual ~CFileInfo();

public:
	CString m_fileName;
	CString m_filePath;
	BOOL m_isFolder;
};

class CDisplayFileInfo : public CFileInfo
{
public:
	CDisplayFileInfo();
	CDisplayFileInfo(CString fileName, BOOL isFolder, CString fileSize, CString filePath);
	virtual ~CDisplayFileInfo();

public:
	CString m_fileSize;
	CString m_fileType;
};


