#pragma once
#include "Common.h"
#include "_IOCPServer.h"
#include "afxcmn.h"
//#include "_CVolume.h"
#include "TrueColorToolBar.h"
#include "afxwin.h"

// CFileManagerDlg 对话框
#define MAKEINT64(a, b) ((unsigned __int64)(((DWORD)(a)) | ((unsigned __int64)((DWORD)(b))) << 32))
typedef struct
{
	DWORD	FileSizeHigh;
	DWORD	FileSizeLow;
}FILE_SIZE;

typedef CList<CString, CString&> List;  
class CFileManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileManagerDlg)

public:
	CFileManagerDlg(CWnd* pParent = NULL, _CIOCPServer* ClientObject = NULL,
		CONTEXT_OBJECT* ContextObject = NULL, ULONG RunMode = PROCESS_MANAGER::SERVER_MODE);   // 标准构造函数
	virtual ~CFileManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL CFileManagerDlg::OnInitDialog();
	VOID CFileManagerDlg::FixedServerVolumeList();
	VOID CFileManagerDlg::FixedClientVolumeList();
	CString CFileManagerDlg::GetParentDirectory(CString FileFullPath);
	VOID CFileManagerDlg::FixedClientFileList(BYTE* BufferData, ULONG BufferLength);
	int	CFileManagerDlg::GetIconIndex(LPCTSTR VolumeName, DWORD FileAttributes);
	VOID CFileManagerDlg::FixedServerFileList(CString DirectoryFullPath = "");
	VOID CFileManagerDlg::GetClientFileList(CString DirectoryFullPath = "");
	void CFileManagerDlg::OnReceiveComplete(void);  //接受完成
	VOID CFileManagerDlg::SendTransferMode();   //发送转换模式的信号
	VOID CFileManagerDlg::EndCopyServerFileToClient();   //停止从服务端拷贝文件到客户端
	
	VOID CFileManagerDlg::SendServerFileDataToClient();
	BOOL CFileManagerDlg::DeleteDirectory(LPCTSTR DirectoryFullPath);
	BOOL CFileManagerDlg::MakeSureDirectoryPathExists(char* DirectoryFullPath);
	//进度条
	__int64  m_Counter;
	void CFileManagerDlg::ShowProgress();


	BOOL CFileManagerDlg::SendServerFileInformationToClient();
	void CFileManagerDlg::EnableControl(BOOL IsEnable);

	afx_msg void OnClose();
	afx_msg void OnNMDblclkListDialogFileManagerServerFile(NMHDR* pNMHDR, LRESULT* pResult);  //右键服务端栏
	afx_msg void OnNMDblclkListDialogFileManagerClientFile(NMHDR* pNMHDR, LRESULT* pResult);  //右键客户端栏
	afx_msg void OnFileManagerServerFilePrevious();											  //服务端文件Previous
	afx_msg void OnFileManagerClientFilePrevious();											  //客户端文件Previous
	afx_msg void OnFileManagerServerFileDelete();											  //服务端文件删除
	afx_msg void OnFileManagerServerFileNewFolder();										  //服务端新建文件夹
	afx_msg void OnFileManagerServerFileStop();												  //服务端文件关闭
	afx_msg void OnFileManagerServerFileViewSmall();										  //服务端文件最小化
	afx_msg void OnFileManagerServerFileViewList();										      //服务端文件视图列表
	afx_msg void OnFileManagerServerFileViewDetail();										  //服务端文件视图编辑

	afx_msg void OnLvnBegindragListDialogFileManagerServerFile(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	VOID CFileManagerDlg::DropFileOnList();
	VOID CFileManagerDlg::CopyServerFileToClient();
	BOOL CFileManagerDlg::FixedServerFileToClient(LPCTSTR DircetoryFullPath);


public:
	_CIOCPServer* m_IOCPServer;
	PCONTEXT_OBJECT  m_ContextObject;
	HICON   	     m_IconHwnd;
	BYTE	         m_ClientData[0x1000];
	BYTE             m_ServerData[0x1000];

	CString m_ServerFileFullPath;
	CString m_ClientFileFullPath;

	//界面相关
	CTrueColorToolBar m_ToolBar_Dialog_File_Manager_Server_File; //两个工具栏
	CTrueColorToolBar m_ToolBar_Dialog_File_Manager_Client_File;
	CImageList* m_CImageList_Large;
	CImageList* m_CImageList_Small;
	CListCtrl m_CListCtrl_Dialog_File_Manager_Server_File;       //两个列表框
	CListCtrl m_CListCtrl_Dialog_File_Manager_Client_File;
	CStatusBar          m_StatusBar;
	CProgressCtrl* m_ProgressCtrl;

	CStatic m_CStatic_Dialog_File_Manager_Server_Position;
	CStatic m_CStatic_Dialog_File_Manager_Client_Position;
	CComboBox m_CComboBox_Dialog_File_Manager_Server_File;   //输入路径使用回车键
	CComboBox m_CComboBox_Dialog_File_Manager_Client_File;


	//文件拖拽拷贝
	HCURSOR    m_CursorHwnd;
	BOOL       m_IsDragging = FALSE;   //拖拽文件
	BOOL       m_IsStop = FALSE;       //拷贝文件的过程中可以停止
	CListCtrl* m_CListCtrl_Drag;
	CListCtrl* m_CListCtrl_Drop;
	List        m_ServerFileToClientJob;   //所有传输任务
	CString     m_SourFileFullPath;        //游走任务
	CString     m_DestinationFileFullPath;
	__int64     m_TransferFileLength;      //当前操作文件总大小

	ULONG  m_TransferMode = TRANSFER_MODE_NORMAL;
};
