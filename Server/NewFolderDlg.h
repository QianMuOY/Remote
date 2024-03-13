#pragma once


// CNewFolderDlg 对话框

class CNewFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewFolderDlg)

public:
	CNewFolderDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewFolderDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_FOLDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_CEdit_Dialog_New_Folder_Main;
};
