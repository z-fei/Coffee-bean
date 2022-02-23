#pragma once
#include "resource.h"
#include <vector>
#include "FileInfo.h"

// CFolderDlg dialog

class CFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderDlg)

public:
	CFolderDlg(CWnd* pParent, std::vector<CDisplayFileInfo> files);   // standard constructor
	virtual ~CFolderDlg();



// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDER_DIALOG };
#endif

protected:
	CListCtrl m_listCtrl;
	CStatic m_totalSize;
	std::vector<CDisplayFileInfo> m_files;

	void InitListData();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
};
