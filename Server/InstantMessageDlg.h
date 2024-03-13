#pragma once
#include "afxwin.h"
#include "_IOCPServer.h"
#include "Common.h"

// CInstantMessageDlg 对话框

class CInstantMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CInstantMessageDlg)

public:
	CInstantMessageDlg(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CInstantMessageDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSTANT_MESSAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CEdit_Main;
private:
	_CIOCPServer* m_IOCPServer;      // 通信对象
	PCONTEXT_OBJECT m_ContextObject; //上下文背景
	HICON  m_IconHwnd;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void CInstantMessageDlg::OnClientSending();
};
