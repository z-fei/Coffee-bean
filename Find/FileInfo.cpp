// FileInfo.cpp : implementation file
//

#include "pch.h"
#include "Find.h"
#include "FileInfo.h"

CDisplayFileInfo::CDisplayFileInfo(CString fileName, BOOL isFolder, CString fileSize, CString filePath) :
	CFileInfo(fileName, filePath, isFolder),
	m_fileSize(fileSize)
{
	m_fileType = m_isFolder ? _T("Folder") : _T("File");
}

CDisplayFileInfo::~CDisplayFileInfo()
{
}

CFileInfo::CFileInfo()
{
}

CFileInfo::CFileInfo(CString fileName, CString filePath, BOOL isFolder) :
	m_fileName(fileName),
	m_filePath(filePath),
	m_isFolder(isFolder)
{
}

CFileInfo::~CFileInfo()
{
}
