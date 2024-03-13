#pragma once
#include "afxcmn.h"
#include "Common.h"

// CProcessModuleManagerDlg 对话框

class CProcessModuleManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessModuleManagerDlg)

public:
	CProcessModuleManagerDlg(CWnd* pParent, ULONG ProcessID, ULONG RunMode);  // 标准构造函数
	virtual ~CProcessModuleManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS_MODULE }  
#endif

private:	
	   HMODULE m_ProcessModuleBase;
	   ULONG   m_ProcessID;
	   ULONG  m_RunMode;
	   HICON  m_IconHwnd;

	   LPFN_SEENUMPROCESSMODULELIST m_SeEnumProcessModuleList = NULL;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL ShowServerProcessModuleList();
	CListCtrl m_CListCtrl_Process_Module;
};
