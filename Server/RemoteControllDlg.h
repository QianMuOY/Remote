#pragma once
#include "_IOCPServer.h"
#include "Common.h"

// CRemoteControllDlg 对话框
extern "C" VOID CopyScreenData(PVOID FirstBufferData, PVOID NextBufferData, ULONG NextBufferLength);
class CRemoteControllDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoteControllDlg)

public:
	CRemoteControllDlg(CWnd* pParent = NULL, _CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT* ContextObject = NULL);   // 标准构造函数
	virtual ~CRemoteControllDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REMOTE_CONTROLL };
#endif

private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
	HDC    m_DesktopDCHandle;
	HDC    m_DesktopMemoryDCHandle;
	LPBITMAPINFO     m_BitmapInfo;
	HBITMAP	m_BitmapHandle;
	PVOID   m_BitmapData;
	POINT   m_ClientCursorPos;   //存储鼠标位置
	ULONG   m_HScrollPos;
	ULONG   m_VScrollPos;
	BOOL    m_IsTraceCursor = FALSE;
	BOOL 	m_IsControl = FALSE;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void OnReceiveComplete(void);
	VOID CRemoteControllDlg::DrawFirstScreen(void);
	VOID CRemoteControllDlg::DrawNextScreen(void);
	VOID CRemoteControllDlg::SetClipboard(char* BufferData, ULONG BufferLength);
	VOID CRemoteControllDlg::SendClipboard();
	VOID CRemoteControllDlg::SendCommand(MSG* Msg);  //发送命令
	BOOL CRemoteControllDlg::SaveSnapshot(void);     //保存截图
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
