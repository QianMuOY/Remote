// DlgRegisterManager.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "RegisterManagerDlg.h"
#include "afxdialogex.h"
#include "Common.h"

// CDlgRegisterManager 对话框
enum MYKEY {
	MHKEY_CLASSES_ROOT,
	MHKEY_CURRENT_USER,
	MHKEY_LOCAL_MACHINE,
	MHKEY_USERS,
	MHKEY_CURRENT_CONFIG
};
enum KEYVALUE {
	MREG_SZ,
	MREG_DWORD,
	MREG_BINARY,
	MREG_EXPAND_SZ
};

IMPLEMENT_DYNAMIC(CRegisterManagerDlg, CDialog)

CRegisterManagerDlg::CRegisterManagerDlg(CWnd* pParent, _CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_REGISTER_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CRegisterManagerDlg::~CRegisterManagerDlg()
{
}

void CRegisterManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DIALOG_REGISTER_MANAGER_SHOW, m_CTreeCtrl_Dialog_Register_Manager_Show);
	DDX_Control(pDX, IDC_LIST_DIALOG_REGISTER_MANAGER_SHOW, m_CListCtrl_Dialog_Register_Manager_Show);
}


BEGIN_MESSAGE_MAP(CRegisterManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIALOG_REGISTER_MANAGER_SHOW, &CRegisterManagerDlg::OnTvnSelchangedTreeDialogRegisterManagerShow)
END_MESSAGE_MAP()


// CRegisterManagerDlg 消息处理程序

void CRegisterManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_REGISTER_MANAGER_PATH_DATA_REPLY:
	{

		AddPath((char*)(m_ContextObject->m_InDeCompressedBufferData.GetArray(1)));
		break;
	}

	case CLIENT_REGISTER_MANAGER_KEY_DATA_REPLY:
	{

		AddKey((char*)(m_ContextObject->m_InDeCompressedBufferData.GetArray(1)));
		break;
	}

	default:
		// 传输发生异常数据
		break;
	}
}



BOOL CRegisterManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_IconHwnd, FALSE);
	//获得客户端IP
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format("\\\\%s - 远程注册表管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);


	// TODO:  在此添加额外的初始化
	 //制作树控件上的图标
	m_CImageList_Tree.Create(18, 18, ILC_COLOR16, 10, 0);
	m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FATHER), IMAGE_ICON, 18, 18, 0);
	m_CImageList_Tree.Add(m_IconHwnd);
	m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FILE), IMAGE_ICON, 18, 18, 0);
	m_CImageList_Tree.Add(m_IconHwnd);


	m_CTreeCtrl_Dialog_Register_Manager_Show.SetImageList(&m_CImageList_Tree, TVSIL_NORMAL);


	m_RootTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("注册表管理", 0, 0, 0, 0);      //0
	m_CurrentUserTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("HKEY_CURRENT_USER", 1, 1, m_RootTreeItem, 0); //1
	m_LocalMachineTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("HKEY_LOCAL_MACHINE", 1, 1, m_RootTreeItem, 0);
	m_UsersTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("HKEY_USERS", 1, 1, m_RootTreeItem, 0);
	m_CurrentConfigTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("HKEY_CURRENT_CONFIG", 1, 1, m_RootTreeItem, 0);
	m_ClassRootTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem("HKEY_CLASSES_ROOT", 1, 1, m_RootTreeItem, 0);

	m_CTreeCtrl_Dialog_Register_Manager_Show.Expand(m_RootTreeItem, TVE_EXPAND);



	m_CListCtrl_Dialog_Register_Manager_Show.InsertColumn(0, "名称", LVCFMT_LEFT, 150, -1);
	m_CListCtrl_Dialog_Register_Manager_Show.InsertColumn(1, "类型", LVCFMT_LEFT, 60, -1);
	m_CListCtrl_Dialog_Register_Manager_Show.InsertColumn(2, "数据", LVCFMT_LEFT, 300, -1);
	m_CListCtrl_Dialog_Register_Manager_Show.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	//////添加图标//////
	m_CImageList_List.Create(16, 16, TRUE, 2, 2);
	m_CImageList_List.Add(AfxGetApp()->LoadIcon(IDI_ICON_STRING));
	m_CImageList_List.Add(AfxGetApp()->LoadIcon(IDI_ICON_DWORD));
	m_CListCtrl_Dialog_Register_Manager_Show.SetImageList(&m_CImageList_List, LVSIL_SMALL);



	m_IsEnable = TRUE;   //该值是为了解决频繁 向被控端请求   
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CRegisterManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);
	}
	CDialog::OnClose();
}
void CRegisterManagerDlg::OnTvnSelchangedTreeDialogRegisterManagerShow(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	if (!m_IsEnable)
	{
		return;
	}
	m_IsEnable = FALSE;;

	//获得你点击的Item
	TVITEM Item = pNMTreeView->itemNew;

	if (Item.hItem == m_RootTreeItem)
	{
		m_IsEnable = TRUE;
		return;
	}


	m_SelectedTreeItem = Item.hItem;			//保存用户打开的子树节点句柄   
	m_CListCtrl_Dialog_Register_Manager_Show.DeleteAllItems();

	CString BufferData = GetFullPath(m_SelectedTreeItem);    //获得键值路径  

	char IsToken = GetFatherPath(BufferData);       //[2] \1\2\3


	//这个while循环是解决反复点击一个选项
	while (m_CTreeCtrl_Dialog_Register_Manager_Show.GetChildItem(Item.hItem) != NULL)
	{
		m_CTreeCtrl_Dialog_Register_Manager_Show.
			DeleteItem(m_CTreeCtrl_Dialog_Register_Manager_Show.GetChildItem(Item.hItem));
	}

	//预加一个键
	int v1 = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, "(默认)", 0);
	m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(v1, 1, "REG_SZ");
	m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(v1, 2, "(数据未设置值)");


	BufferData.Insert(0, IsToken);//插入  那个根键
	IsToken = CLIENT_REGISTER_MANAGER_DATA_CONTINUE;
	BufferData.Insert(0, IsToken);      //插入查询命令  [COMMAND_REG_FIND][x]

	m_IOCPServer->OnClientPreSending(m_ContextObject, (LPBYTE)(BufferData.GetBuffer(0)),
		BufferData.GetLength() + 1);


	m_IsEnable = TRUE;
	*pResult = 0;
}
CString CRegisterManagerDlg::GetFullPath(HTREEITEM SelectedTreeItem)
{
	CString v1;
	CString v2 = "";
	while (1)
	{
		if (SelectedTreeItem == m_RootTreeItem)
		{
			return v2;
		}
		v1 = m_CTreeCtrl_Dialog_Register_Manager_Show.GetItemText(SelectedTreeItem);
		if (v1.Right(1) != "\\")
			v1 += "\\";
		v2 = v1 + v2;
		SelectedTreeItem = m_CTreeCtrl_Dialog_Register_Manager_Show.GetParentItem(SelectedTreeItem);   //得到父的

	}
	return v2;
}
char CRegisterManagerDlg::GetFatherPath(CString& FullPath)
{
	char IsToken;
	if (!FullPath.Find("HKEY_CLASSES_ROOT"))	//判断主键
	{

		IsToken = MHKEY_CLASSES_ROOT;
		FullPath.Delete(0, sizeof("HKEY_CLASSES_ROOT"));
	}
	else if (!FullPath.Find("HKEY_CURRENT_USER"))
	{
		IsToken = MHKEY_CURRENT_USER;
		FullPath.Delete(0, sizeof("HKEY_CURRENT_USER"));

	}
	else if (!FullPath.Find("HKEY_LOCAL_MACHINE"))
	{
		IsToken = MHKEY_LOCAL_MACHINE;
		FullPath.Delete(0, sizeof("HKEY_LOCAL_MACHINE"));

	}
	else if (!FullPath.Find("HKEY_USERS"))
	{
		IsToken = MHKEY_USERS;
		FullPath.Delete(0, sizeof("HKEY_USERS"));

	}
	else if (!FullPath.Find("HKEY_CURRENT_CONFIG"))
	{
		IsToken = MHKEY_CURRENT_CONFIG;
		FullPath.Delete(0, sizeof("HKEY_CURRENT_CONFIG"));

	}
	return IsToken;
}
void CRegisterManagerDlg::AddPath(char* BufferData)
{

	if (BufferData == NULL) return;
	int PacketHeaderLength = sizeof(PACKET_HEADER);
	PACKET_HEADER PacketHeader;
	memcpy((void*)&PacketHeader, BufferData, PacketHeaderLength);
	DWORD v1 = PacketHeader.v1;
	int v2 = PacketHeader.v2;

	if (v2 > 0 && v1 > 0) {                   //一点保护措施
		for (int i = 0; i < v1; i++) {
			char* PathName = BufferData + v2 * i + PacketHeaderLength;
			m_CTreeCtrl_Dialog_Register_Manager_Show.InsertItem(PathName, 1, 1, m_SelectedTreeItem, 0);//插入子键名称
			m_CTreeCtrl_Dialog_Register_Manager_Show.Expand(m_SelectedTreeItem, TVE_EXPAND);
		}
	}
}
void CRegisterManagerDlg::AddKey(char* BufferData)
{
	m_CListCtrl_Dialog_Register_Manager_Show.DeleteAllItems();
	int Item = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, "(Data)", 0);
	m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 1, "REG_SZ");
	m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 2, "(NULL)");

	if (BufferData == NULL) return;

	PACKET_HEADER PacketHeader;
	memcpy((void*)&PacketHeader, BufferData, sizeof(PACKET_HEADER));
	char* Offset = BufferData + sizeof(PACKET_HEADER);
	for (int i = 0; i < PacketHeader.v1; i++)
	{
		BYTE Type = Offset[0];   //类型
		Offset += sizeof(BYTE);
		char* ValueName = Offset;   //取出名字
		Offset += PacketHeader.v2;
		BYTE* ValueDate = (BYTE*)Offset;      //取出值
		Offset += PacketHeader.v3;
		if (Type == MREG_SZ)
		{
			int Item = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, ValueName, 0);
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 1, "REG_SZ");
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 2, (char*)ValueDate);
		}
		if (Type == MREG_DWORD)
		{

			char v1[256];
			DWORD v2 = 0;
			memcpy((void*)&v2, ValueDate, sizeof(DWORD));
			CString v3;
			v3.Format("0x%x", v2);

			int Item = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, ValueName, 1);
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 1, "REG_DWORD");
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 2, v3);

		}
		if (Type == MREG_BINARY)
		{
			char v1[256];
			sprintf(v1, "%d", ValueDate);

			int Item = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, ValueName, 1);
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 1, "REG_BINARY");
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 2, v1);
		}
		if (Type == MREG_EXPAND_SZ)
		{
			int Item = m_CListCtrl_Dialog_Register_Manager_Show.InsertItem(0, ValueName, 0);
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 1, "REG_EXPAND_SZ");
			m_CListCtrl_Dialog_Register_Manager_Show.SetItemText(Item, 2, (char*)ValueDate);
		}
	}
}