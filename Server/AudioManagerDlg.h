#pragma once
#include "_IOCPServer.h"
#include "Common.h"
#include "afxcmn.h"
#include "Audio.h"
// CAudioManagerDlg 对话框

class CAudioManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CAudioManagerDlg)

public:
	CAudioManagerDlg(CWnd* pParent = NULL, _CIOCPServer* ClientObject = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CAudioManagerDlg();
	static DWORD  CAudioManagerDlg::ThreadProcedure(LPVOID ParameterData);
private:
	_CIOCPServer* m_ClientObject;
	PCONTEXT_OBJECT m_ContextObject;
	HICON   	m_IconHwnd;
	CAudio      m_Audio;
	HANDLE      m_ThreadHandle;
	BOOL        m_IsLoop;


	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUDIO_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void CAudioManagerDlg::OnReceiveComplete(void);
	CButton m_Check_Dialog_Audio_Manager_To_Client;
};





