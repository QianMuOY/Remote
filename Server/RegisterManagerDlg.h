#pragma once
#include "_IOCPServer.h"
#include "Common.h"
#include "afxcmn.h"

// CDlgRegisterManager 对话框
struct PACKET_HEADER {
	int   v1;          //名字个数
	DWORD v2;          //名字大小
	DWORD v3;          //值大小

};
class CRegisterManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterManagerDlg)

public:
	CRegisterManagerDlg(CWnd* pParent = NULL, _CIOCPServer* ClientObject = NULL, PCONTEXT_OBJECT ContextObject = NULL);   // 标准构造函数
	virtual ~CRegisterManagerDlg();
private:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON   	m_IconHwnd;
	HTREEITEM	m_RootTreeItem;
	HTREEITEM	m_CurrentUserTreeItem;
	HTREEITEM	m_LocalMachineTreeItem;
	HTREEITEM	m_UsersTreeItem;
	HTREEITEM	m_CurrentConfigTreeItem;
	HTREEITEM	m_ClassRootTreeItem;
	BOOL        m_IsEnable;
	HTREEITEM   m_SelectedTreeItem;
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER_MANAGER };
#endif
public:
	void CRegisterManagerDlg::OnReceiveComplete(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_CTreeCtrl_Dialog_Register_Manager_Show;
	CImageList	m_CImageList_Tree;   //树控件上的图标	
	CListCtrl m_CListCtrl_Dialog_Register_Manager_Show;
	CImageList	m_CImageList_List;   //List控件上的图标
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTvnSelchangedTreeDialogRegisterManagerShow(NMHDR* pNMHDR, LRESULT* pResult);
	CString CRegisterManagerDlg::GetFullPath(HTREEITEM SelectedTreeItem);
	char CRegisterManagerDlg::GetFatherPath(CString& FullPath);
	void CRegisterManagerDlg::AddPath(char* BufferData);
	void CRegisterManagerDlg::AddKey(char* BufferData);
};
