// FolderDlg.cpp : implementation file
//

#include "pch.h"
#include "FolderDlg.h"
#include "afxdialogex.h"


// CFolderDlg dialog

IMPLEMENT_DYNAMIC(CFolderDlg, CDialog)

CFolderDlg::CFolderDlg(CWnd* pParent, std::vector<CDisplayFileInfo> files)
	: CDialog(IDD_FOLDER_DIALOG, pParent),
	  m_files(files)
{
}

CFolderDlg::~CFolderDlg()
{
}

BOOL CFolderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Insert column
	m_listCtrl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(1, _T("Size"), LVCFMT_LEFT, 50);
	m_listCtrl.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 50);
	m_listCtrl.InsertColumn(3, _T("Path"), LVCFMT_LEFT, 200);

	int index = 0;
	for (std::vector<CDisplayFileInfo>::iterator it = m_files.begin(); it < m_files.end(); ++it)
	{
		m_listCtrl.InsertItem(index, (LPCTSTR)it->m_fileName);
		m_listCtrl.SetItemText(index, 1, (LPCTSTR)it->m_fileSize);
		m_listCtrl.SetItemText(index, 2, (LPCTSTR)it->m_fileType);
		m_listCtrl.SetItemText(index, 3, (LPCTSTR)it->m_filePath);
		index++;
	}

	return TRUE;
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)
END_MESSAGE_MAP()


// CFolderDlg message handlers
