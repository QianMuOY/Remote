#pragma once


// CCreateClientDlg 对话框

class CCreateClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateClientDlg)

public:
	CCreateClientDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateClientDlg();
	afx_msg void OnBnClickedOk();
	int CCreateClientDlg::MemoryFind(const char* BufferData, const char* KeyValue, int BufferLength, int KeyLength);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CREATE_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_CEdit_CreateClient_IP;
	CString m_CEdit_CreateClient_Port;


};
