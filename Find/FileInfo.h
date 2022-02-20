#pragma once


// FileInfo
class CFileInfoBase
{
public:
	CFileInfoBase();
	CFileInfoBase(CString fileName, BOOL isFolder);
	virtual ~CFileInfoBase();

public:
	CString m_fileName;
	BOOL m_isFolder;
};

class CUSNFileInfo : public CFileInfoBase
{
public:
	CUSNFileInfo();
	CUSNFileInfo(CString fileName, BOOL isFolder, DWORDLONG fileRefID, DWORDLONG fileParentRefID, CString rootPath);
	virtual ~CUSNFileInfo();

public:
	DWORDLONG m_fileRefID;
	DWORDLONG m_fileParentRefID;
	CString m_rootPath;
};

class CDisplayFileInfo : public CFileInfoBase
{
public:
	CDisplayFileInfo();
	CDisplayFileInfo(CString fileName, BOOL isFolder, CString fileSize, CString filePath);
	virtual ~CDisplayFileInfo();

public:
	CString m_fileSize;
	CString m_filePath;
	CString m_fileType;
};


