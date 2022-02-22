
// FindDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Find.h"
#include "FindDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <thread>


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFindDlg dialog



CFindDlg::CFindDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FIND_DIALOG, pParent),
	  m_functionManager(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, m_listControl);
	DDX_Control(pDX, IDC_EDIT1, m_folderPath);
	DDX_Control(pDX, IDC_STATIC_STATE, m_state);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CFindDlg::OnBnClickedButtonSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFindDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CFindDlg message handlers
BOOL CFindDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
}


BOOL CFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_progress.SetRange(0, 100);
	OnInitListControl();
	InitFunction();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFindDlg::OnInitListControl()
{
	//Insert column
	m_listControl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 100);
	m_listControl.InsertColumn(1, _T("Size"), LVCFMT_LEFT, 50);
	m_listControl.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 50);
	m_listControl.InsertColumn(3, _T("Path"), LVCFMT_LEFT, 200);
}

void CFindDlg::InitFunction()
{
	m_functionManager.AddFunction(SEARCH_EVENT(INIT_DISK_COMPLETE), OnInitDiskComplete);
	m_functionManager.AddFunction(SEARCH_EVENT(FILE_SEARCH_COMPLETE), OnFileSearchComplete);
	m_functionManager.AddFunction(SEARCH_EVENT(FOLDER_SEARCH_COMPLETE), OnFolderSearchComplete);
	m_functionManager.AddFunction(SEARCH_EVENT(UPDATE_PROGRESS), OnProgressBarUpdate);

	m_functionManager.OnInitDisk();
}

void CFindDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFindDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFindDlg::OnFileSearchComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress)
{
	int index = 0;
	auto pDlg = (CFindDlg*)lp;
	pDlg->m_listControl.DeleteAllItems();
	for (auto it = files.begin(); it < files.end(); ++it)
	{
		pDlg->m_listControl.InsertItem(index, it->m_fileName);
		pDlg->m_listControl.SetItemText(index, 1, it->m_fileSize);
		pDlg->m_listControl.SetItemText(index, 2, it->m_fileType);
		pDlg->m_listControl.SetItemText(index, 3, it->m_filePath);
		index++;
	}

	pDlg->m_state.SetWindowTextW(_T("State: Ready!"));
	pDlg->m_progress.SetPos(progress);
	pDlg->m_progress.ShowWindow(FALSE);
}

void CFindDlg::OnFolderSearchComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress)
{
	auto pDlg = (CFindDlg*)lp;

	pDlg->m_state.SetWindowTextW(_T("State: Ready!"));
	pDlg->m_progress.ShowWindow(FALSE);

	CFolderDlg folderDlg(pDlg, files);
	folderDlg.DoModal();
}

void CFindDlg::OnInitDiskComplete(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress)
{
	auto pDlg = (CFindDlg*)lp;
	pDlg->m_state.SetWindowTextW(_T("State: Ready!"));
	pDlg->m_progress.ShowWindow(FALSE);
}

void CFindDlg::OnProgressBarUpdate(LPVOID lp, std::vector<CDisplayFileInfo> files, INT progress)
{
	auto pDlg = (CFindDlg*)lp;
	pDlg->m_progress.SetPos(100 * progress / 100000);
}

void CFindDlg::OnBnClickedButtonSearch()
{
	CString str;
	m_folderPath.GetWindowTextW(str);
	if (str.IsEmpty()) return;
	if (m_functionManager.OnSearch(str))
	{
		m_state.SetWindowTextW(_T("State: Searching file..."));
		m_progress.SetPos(0);
		m_progress.ShowWindow(TRUE);
	}
	else
	{
		AfxMessageBox(_T("Please wait for init"));
	}
}

void CFindDlg::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_listControl.GetSelectionMark();
	if (index == -1) return;

	CString path = m_listControl.GetItemText(index, 3);
	if ((GetFileAttributes(path) & FILE_ATTRIBUTE_DIRECTORY) == 0) return;

	m_functionManager.OnSearchFolder(path);
	m_state.SetWindowTextW(_T("State: Searching folder..."));

	*pResult = 0;
}
