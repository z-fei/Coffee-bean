
// FindDlg.h : header file
//

#pragma once
#include "FunctionManager.h"
#include "FolderDlg.h"

// CFindDlg dialog
class CFindDlg : public CDialogEx
{
// Construction
public:
	CFindDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIND_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	CListCtrl m_listControl;
	CEdit m_folderPath;
	CStatic m_state;
	CProgressCtrl m_progress;

	CFunctionManager m_functionManager;

	// Generated message map functions
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	void OnInitListControl();
	void InitFunction();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	static void OnFileSearchComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress);
	static void OnFolderSearchComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress);
	static void OnInitDiskComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress);
	static void OnProgressBarUpdate(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress);
};
