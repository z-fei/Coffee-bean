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

	InitListData();

	return TRUE;
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_STATIC_SIZE, m_totalSize);
}

void CFolderDlg::InitListData()
{
	//Insert column
	m_listCtrl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(1, _T("Size"), LVCFMT_LEFT, 100);
	m_listCtrl.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 50);
	m_listCtrl.InsertColumn(3, _T("Path"), LVCFMT_LEFT, 200);
	m_listCtrl.InsertItem(0, LPSTR_TEXTCALLBACK);
	m_listCtrl.InsertItem(1, LPSTR_TEXTCALLBACK);
	m_listCtrl.InsertItem(2, LPSTR_TEXTCALLBACK);
	m_listCtrl.InsertItem(3, LPSTR_TEXTCALLBACK);

	if (m_files.size() > 0)
		m_listCtrl.SetItemCountEx((int)(m_files.size() - 1), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

	m_totalSize.SetWindowTextW(_T("Total size: ") + m_files.back().m_fileSize);
}


BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, &CFolderDlg::OnLvnGetdispinfoList)
END_MESSAGE_MAP()


// CFolderDlg message handlers
void CFolderDlg::OnLvnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* pItem = &(pDispInfo)->item;
	int iItemIndx = pItem->iItem;

	if (pItem->mask & LVIF_TEXT)
	{
		switch (pItem->iSubItem) {
		case 0: 
			lstrcpy(pItem->pszText,	m_files.at(iItemIndx).m_fileName);
			break;
		case 1: 
			lstrcpy(pItem->pszText, m_files.at(iItemIndx).m_fileSize);
			break;
		case 2: 
			lstrcpy(pItem->pszText, m_files.at(iItemIndx).m_fileType);
			break;
		case 3:
			lstrcpy(pItem->pszText, m_files.at(iItemIndx).m_filePath);
			break;
		}
	}

	*pResult = 0;
}
