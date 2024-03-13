#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "_IOCPServer.h"


// CProcessManagerDlg 对话框

class CProcessManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessManagerDlg)

public:
	CProcessManagerDlg(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT* ContextObject = NULL, ULONG RunMode = PROCESS_MANAGER::SERVER_MODE);   
	virtual ~CProcessManagerDlg();
	void CProcessManagerDlg::OnReceiveComplete(void);
	BOOL CProcessManagerDlg::OnInitDialog(); //初始化对话框
	BOOL CProcessManagerDlg::ShowClientProcessList();
	BOOL CProcessManagerDlg::ShowServerProcessList();
	void CProcessManagerDlg::GetClientProcessList();


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	HMODULE m_ProcessModuleBase;
	LPFN_SEENUMPROCESSLIST m_SeEnumProcessList;
	pfnRing3KillProcess  m_Ring3KillProcess;
	HICON  m_IconHwnd;

	ULONG  m_RunMode;
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
public:
	CListCtrl m_CListCtrl_Client_Process;
	CListCtrl m_CListCtrl_Server_Process;
	afx_msg void OnNMCustomdrawListServerProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnServerProcessRefresh();
	afx_msg void OnServerProcessLocation();
	VOID LocationExplorer(CString ProcessFullPath);
	afx_msg void OnNMRClickListServerProcess(NMHDR* pNMHDR, LRESULT* pResult); //服务端进程

	afx_msg void OnNMRClickListClientProcess(NMHDR* pNMHDR, LRESULT* pResult); //客户端进程
	afx_msg void OnClientProcessRefresh();//客户端刷新
	afx_msg void OnClientProcessRing3Kill(); //客户端环3结束进程
	afx_msg void OnServerProcessRing3Kill(); //服务端环3结束进程
	afx_msg void OnServerProcessModule();


};
