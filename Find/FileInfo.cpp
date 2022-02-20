// FileInfo.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FileInfo.h"

//CFileInfoBase
CFileInfoBase::CFileInfoBase() :
	m_isFolder(FALSE)
{
}

CFileInfoBase::CFileInfoBase(CString fileName, BOOL isFolder) :
	m_fileName(fileName),
	m_isFolder(isFolder)
{
}

CFileInfoBase::~CFileInfoBase()
{
}

//CUSNFileInfo
CUSNFileInfo::CUSNFileInfo() :
	m_fileRefID(0),
	m_fileParentRefID(0)
{
}

CUSNFileInfo::CUSNFileInfo(CString fileName, BOOL isFolder, DWORDLONG fileRefID, DWORDLONG fileParentRefID, CString rootPath) :
	CFileInfoBase(fileName, isFolder),
	m_fileRefID(fileRefID),
	m_fileParentRefID(fileParentRefID),
	m_rootPath(rootPath)
{
}

CUSNFileInfo::~CUSNFileInfo()
{
}

CDisplayFileInfo::CDisplayFileInfo()
{
}

CDisplayFileInfo::CDisplayFileInfo(CString fileName, BOOL isFolder, CString fileSize, CString filePath) :
	CFileInfoBase(fileName, isFolder),
	m_fileSize(fileSize),
	m_filePath(filePath)
{
	m_fileType = m_isFolder ? _T("Folder") : _T("File");
}

CDisplayFileInfo::~CDisplayFileInfo()
{
}
