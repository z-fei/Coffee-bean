#pragma once
#include "resource.h"
#include <vector>
#include "FileInfo.h"

// CFolderDlg dialog

class CFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderDlg)

public:
	CFolderDlg(CWnd* pParent, std::vector<CDisplayFileInfo>);   // standard constructor
	virtual ~CFolderDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDER_DIALOG };
#endif

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CListCtrl m_listCtrl;
	std::vector<CDisplayFileInfo> m_files;
	DECLARE_MESSAGE_MAP()
};
