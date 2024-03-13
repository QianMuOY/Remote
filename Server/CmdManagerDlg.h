#pragma once
#include "Common.h"
#include "_IOCPServer.h"
#include "afxwin.h"

// CCmdManagerDlg 对话框

class CCmdManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdManagerDlg)

public:
	CCmdManagerDlg(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT* ContextObject = NULL);   // 标准构造函数
	virtual ~CCmdManagerDlg();
	virtual BOOL OnInitDialog();
	void CCmdManagerDlg::OnReceiveComplete(void);
	VOID CCmdManagerDlg::ShowClientCmdData(void);
	afx_msg void OnClose();   //关闭窗口
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	ULONG           m_911;   //获得当前数据所在位置;
	ULONG           m_ShowDataLength;
private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CMD_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CEdit_Cmd_Main;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
