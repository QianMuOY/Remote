#pragma once


// CDlgServerSet 对话框
#include "ServerSetManager.h"
#include "afxwin.h"


class CDlgServerSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgServerSet)

public:
	CDlgServerSet(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgServerSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SERVER_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	long m_CEdit_Dialog_Server_Max_Connect_Num;  //最大连接数
	long m_CEdit_Dialog_Server_Listen_Port;      //监听端口
	virtual BOOL OnInitDialog();  //初始化对话框
	afx_msg void OnBnClickedButtonDialogServerSetApply();
	afx_msg void OnEnChangeEditDialogServerMaxConnectNum();
	afx_msg void OnEnChangeEditDialogServerListenPort();
	CButton m_Button_Dialog_Server_Set_Apply;
};
