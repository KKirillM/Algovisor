#include "stdafx.h"
#include "PrivateKey.h"


// PrivateKey dialog

IMPLEMENT_DYNAMIC(PrivateKey, CDialogEx)

PrivateKey::PrivateKey(CWnd* pParent /*=NULL*/)
	: CDialogEx(PrivateKey::IDD, pParent)
{
}

PrivateKey::~PrivateKey()
{
}

void PrivateKey::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEY_ICON, logo);
}


BEGIN_MESSAGE_MAP(PrivateKey, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &PrivateKey::OnBnClickedFindFilePath)
	ON_BN_CLICKED(IDOK, &PrivateKey::OnBnClickedOk)
END_MESSAGE_MAP()


// PrivateKey message handlers


BOOL PrivateKey::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hBitmap = (HICON)LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_KEY_ICON), IMAGE_ICON, 67, 81, 0);
	logo.SetIcon(hBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void PrivateKey::OnBnClickedFindFilePath()
{
	TCHAR BASED_CODE szFilter[] = L"Key Files (*.cpr)|*.cpr|";

	CFileDialog pathDlg(TRUE, L"cpr", NULL, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilter, this);

	if(pathDlg.DoModal() == IDOK)
	{
		CString pathName = pathDlg.GetPathName();
		//CString fileName = pathDlg.GetFileTitle ();
		GetDlgItem(IDC_KEY_CPR_PATH)->SetWindowTextW(pathName);
	}

	//на всякий случай очистим строку с ошибкой
	//GetDlgItem(STATIC_PATH_ERROR_MESSAGE)->SetWindowTextW(L"");
}


void PrivateKey::OnBnClickedOk()
{
	//проверяем есть ли данные о пути
	CString wExistingFileName;
	GetDlgItem(IDC_KEY_CPR_PATH)->GetWindowText(wExistingFileName);
	if (wExistingFileName == L"")
	{
		GetDlgItem(STATIC_PATH_ERROR_MESSAGE)->SetWindowTextW(L"Enter the path");
		return;
	}

	//смотрим есть ли по этому пути файл
	OFSTRUCT reOpenBuff;
	CStringA aExistingFileName(wExistingFileName);
	HFILE hf = OpenFile(aExistingFileName, &reOpenBuff, OF_EXIST);
	if (hf == HFILE_ERROR)
	{
		GetDlgItem(STATIC_PATH_ERROR_MESSAGE)->SetWindowTextW(L"File not found");
		return;
	}

	//файл существует по указанному пути. копируем его в папку программы
	CString wNewFileName;
	TCHAR direcotry[MAX_PATH];
	if(!GetCurrentDirectory(MAX_PATH, direcotry))
	{
		GetDlgItem(STATIC_PATH_ERROR_MESSAGE)->SetWindowTextW(L"The file path is too long");
		return;
	}
	wNewFileName.Format(L"%s\\key.cpr", direcotry);

	//проверяем на совпадение полученного пути с папкой программы
	if (wExistingFileName == wNewFileName)
		CDialogEx::OnOK();
	
	if (!CopyFile(wExistingFileName, wNewFileName, FALSE))
	{
		GetDlgItem(STATIC_PATH_ERROR_MESSAGE)->SetWindowTextW(L"Copy file error");
		return;
	}

	CDialogEx::OnOK();
}
