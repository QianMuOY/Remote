// NewFolderDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "NewFolderDlg.h"
#include "afxdialogex.h"


// CNewFolderDlg 对话框

IMPLEMENT_DYNAMIC(CNewFolderDlg, CDialog)

CNewFolderDlg::CNewFolderDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_NEW_FOLDER, pParent)
	, m_CEdit_Dialog_New_Folder_Main(_T(""))
{

}

CNewFolderDlg::~CNewFolderDlg()
{
}

void CNewFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIALOG_NEW_FOLDER_MAIN, m_CEdit_Dialog_New_Folder_Main);
}


BEGIN_MESSAGE_MAP(CNewFolderDlg, CDialog)
END_MESSAGE_MAP()


// CNewFolderDlg 消息处理程序
