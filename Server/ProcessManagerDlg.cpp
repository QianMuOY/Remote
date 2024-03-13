// ProcessManagerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ProcessManagerDlg.h"
#include "ProcessModuleManagerDlg.h"
#include "afxdialogex.h"
#include "Common.h"



// CProcessManagerDlg 对话框
IMPLEMENT_DYNAMIC(CProcessManagerDlg, CDialog)
CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent, _CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject, ULONG RunMode)
	: CDialog(IDD_DIALOG_PROCESS_MANAGER, pParent)
{
	m_RunMode = RunMode;  //运行模式
	 
	m_IOCPServer = IOCPServer;   //通信服务
	m_ContextObject = ContextObject;  //上下背景文

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); //Icon
	m_ProcessModuleBase = LoadLibrary("Process.dll");  
	m_SeEnumProcessList = (LPFN_SEENUMPROCESSLIST)GetProcAddress(m_ProcessModuleBase, "SeEnumProcessList");
	m_Ring3KillProcess = (pfnRing3KillProcess)GetProcAddress(m_ProcessModuleBase, "Ring3KillProcess");

	Wow64EnableWow64FsRedirection(FALSE);
}
//析构
CProcessManagerDlg::~CProcessManagerDlg()
{
	if (m_ProcessModuleBase != NULL)
	{
		FreeLibrary(m_ProcessModuleBase);
		m_ProcessModuleBase = NULL;

		m_SeEnumProcessList = NULL;
		m_Ring3KillProcess = NULL;
	}
}
//触发数据改变
void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_PROCESS, m_CListCtrl_Client_Process);
	DDX_Control(pDX, IDC_LIST_SERVER_PROCESS, m_CListCtrl_Server_Process);
}

//关联消息和事件处理程序
BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SERVER_PROCESS, &CProcessManagerDlg::OnNMCustomdrawListServerProcess)
	ON_COMMAND(ID_SERVERPROCESS_REFRESH, &CProcessManagerDlg::OnServerProcessRefresh)
	ON_COMMAND(ID_SERVERPROCESS_LOCATION, &CProcessManagerDlg::OnServerProcessLocation)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER_PROCESS, &CProcessManagerDlg::OnNMRClickListServerProcess)
	ON_WM_CLOSE()

	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_PROCESS, &CProcessManagerDlg::OnNMRClickListClientProcess)  //客户端 菜单
	ON_COMMAND(ID_CLIENTPROCESS_REFRESH, &CProcessManagerDlg::OnClientProcessRefresh)     //客户端刷新
	ON_COMMAND(ID_CLIENTPROCESS_RING3_KILL, &CProcessManagerDlg::OnClientProcessRing3Kill)  //客户端环3杀
	ON_COMMAND(ID_SERVERPROCESS_RING3_KILL, &CProcessManagerDlg::OnServerProcessRing3Kill)   //服务端环3杀
	ON_COMMAND(ID_SERVERPROCESS_MODULE, &CProcessManagerDlg::OnServerProcessModule)    //服务端进程模块
END_MESSAGE_MAP()

// CProcessManagerDlg 消息处理程序
void CProcessManagerDlg::OnReceiveComplete(void)
{
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_PROCESS_MANAGER_REPLY:
	{

		ShowClientProcessList();
		break;
	}
	case  CLIENT_PROCESS_KILL_REPLY:
	{

		GetClientProcessList();
		break;
	}
	default:
	{
		break;
	}
	}
	return;
}
//初始化对话框
BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_IconHwnd, FALSE);   //设置忍者图标到Dialog

	// 获得客户端IP
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);

	BOOL IsOk = INVALID_SOCKET;
	if (m_ContextObject != NULL)
	{
		IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	}
	v1.Format("%s - 远程进程管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
	// 设置对话框标题
	SetWindowText(v1);//设置对话框标题



	LOGFONT  Logfont;//最好弄成类成员,全局变量,静态成员  
	CFont* v3 = m_CListCtrl_Server_Process.GetFont();
	v3->GetLogFont(&Logfont);
	Logfont.lfHeight = Logfont.lfHeight * 1.3;   //这里可以修改字体的高比例
	Logfont.lfWidth = Logfont.lfWidth * 1.3;     //这里可以修改字体的宽比例
	CFont  v4;
	v4.CreateFontIndirect(&Logfont);
	m_CListCtrl_Server_Process.SetFont(&v4);
	m_CListCtrl_Client_Process.SetFont(&v4);
	v4.Detach();


	switch (m_RunMode)
	{
		//Servrt Client
	case  PROCESS_MANAGER::CLIENT_MODE:
	{

		//设置显示ListCtrl属性
		m_CListCtrl_Server_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Process.InsertColumn(1, "进程镜像", LVCFMT_LEFT, 100);
		m_CListCtrl_Server_Process.InsertColumn(2, "程序路径", LVCFMT_LEFT, 700);
		m_CListCtrl_Server_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);

		m_CListCtrl_Client_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Client_Process.InsertColumn(1, "进程镜像", LVCFMT_LEFT, 100);
		m_CListCtrl_Client_Process.InsertColumn(2, "程序路径", LVCFMT_LEFT, 700);
		m_CListCtrl_Client_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerProcessList();
		ShowClientProcessList();


		break;
	}
	//Server
	case PROCESS_MANAGER::SERVER_MODE:
	{
		m_CListCtrl_Client_Process.EnableWindow(FALSE);   //禁止功能
		m_CListCtrl_Client_Process.ShowWindow(FALSE);     //隐藏
		RECT Rect;
		GetWindowRect(&Rect);  //当前Dialog大小
		SetWindowPos(NULL, 0, 0, Rect.right / 2, Rect.bottom, SWP_NOMOVE);

		//设置显示ListCtrl属性
		m_CListCtrl_Server_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Process.InsertColumn(1, "进程镜像", LVCFMT_LEFT, 100);
		m_CListCtrl_Server_Process.InsertColumn(2, "程序路径", LVCFMT_LEFT, 700);
		m_CListCtrl_Server_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerProcessList();
		break;
	}
	default:
		break;
	}
	//C:\Windows\System32\calc.exe mspaint.exe 
	//C:\Windows\SysWow64\calc.exe

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
//展示客户端的进程信息列表
BOOL CProcessManagerDlg::ShowClientProcessList()
{
	char* BufferData = (char*)(m_ContextObject->m_InDeCompressedBufferData.GetArray(1)); //xiaoxi[][][][][]
	char* ImageNameData;   //notepad.exe  
	char* ProcessFullPathData;
	DWORD	Offset = 0;
	CString v1;
	m_CListCtrl_Client_Process.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	int i;
	for (i = 0; Offset < m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1; i++)
	{
		HANDLE*	ProcessID = (HANDLE*)(BufferData + Offset);        //这里得到进程ID
		ImageNameData = BufferData + Offset + sizeof(DWORD);         //进程名就是ID之后的啦
		ProcessFullPathData = ImageNameData + lstrlen(ImageNameData) + 1;    //完整名就是进程名之后的啦
																 //他的数据结构的构建很巧妙
		v1.Format("%5u", *ProcessID);
		m_CListCtrl_Client_Process.InsertItem(i, v1);       //将得到的数据加入到列表当中

		m_CListCtrl_Client_Process.SetItemText(i, 1, ImageNameData);
		m_CListCtrl_Client_Process.SetItemText(i, 2, ProcessFullPathData);
		// ItemData 为进程ID
		m_CListCtrl_Client_Process.SetItemData(i, (DWORD_PTR)*ProcessID);   //进程ID 设置Hide
		Offset += sizeof(HANDLE) + lstrlen(ImageNameData) + lstrlen(ProcessFullPathData) + 2;   //跳过这个数据结构 进入下一个循环
	}

	//4System.exe\0C:\windows\system.exe\0\08notepad.exe\0c:\windows\notepad.exe\0\0
	v1.Format("程序个数 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Client_Process.SetColumn(2, &lvc); //在列表中显示有多少个进程
	return TRUE;
}
//显示服务端进程信息列表
BOOL CProcessManagerDlg::ShowServerProcessList()
{
	//枚举
	ULONG ItemCount = 0;
	vector<_PROCESS_INFORMATION_> ProcessInfo;
	if (m_SeEnumProcessList == NULL)
	{
		return FALSE;
	}
	ItemCount = m_SeEnumProcessList(ProcessInfo);
	if (ItemCount == 0)
	{
		return FALSE;
	}

	char* ImageNameData;   //notepad.exe  
	char* ProcessFullPathData;

	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Server_Process.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	vector<_PROCESS_INFORMATION_>::iterator i;
	int j = 0;
	for (i = ProcessInfo.begin(); i != ProcessInfo.end(); i++)
	{
		ULONG32 ProcessID = i->ProcessID;
		v1.Format("%5u", ProcessID);
		ImageNameData = i->ImageNameData;
		ProcessFullPathData = i->ProcessFullPathData;

		m_CListCtrl_Server_Process.InsertItem(j, v1);       //将得到的数据加入到列表当中

		m_CListCtrl_Server_Process.SetItemText(j, 1, ImageNameData);
		m_CListCtrl_Server_Process.SetItemText(j, 2, ProcessFullPathData);
		// ItemData 为进程ID
		m_CListCtrl_Server_Process.SetItemData(j, ProcessID);   //进程ID 设置Hide

		j++;
	}

	v1.Format("程序个数 / %d", j);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Server_Process.SetColumn(2, &lvc); //在列表中显示有多少个进程
	return  TRUE;
}
//发送获取客户端进程信息列表请求
void CProcessManagerDlg::GetClientProcessList()
{
	BYTE IsToken = CLIENT_PROCESS_REFRESH_REQUIRE;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, 1);
}
//在列表中显示服务端进程信息
void CProcessManagerDlg::OnNMCustomdrawListServerProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
		// Take the default processing unless we set this to something else below.  
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint  
	// stage, then tell Windows we want messages for every item.  
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		// This is the notification message for an item.  We'll request  
		// notifications before each subitem's prepaint stage.  
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{

		COLORREF clrNewTextColor, clrNewBkColor;

		int    iItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CString ProcessImageName = m_CListCtrl_Server_Process.GetItemText(iItem, 1);
		if (strcmp(ProcessImageName, "demo.exe") == 0)
		{
			clrNewTextColor = RGB(0, 0, 0);       //Set the text   
			clrNewBkColor = RGB(0, 255, 255);     //青色  
		}
		else if (strcmp(ProcessImageName, "Taskmgr.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text   
			clrNewBkColor = RGB(255, 255, 0);     //黄色  
		}
		else if (strcmp(ProcessImageName, "Calculator.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 153, 0);     //咖啡色  
		}
		else if (strcmp(ProcessImageName, "mspaint.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 66, 255);            //粉红色  
		}
		else if (strcmp(ProcessImageName, "explorer.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 0, 0);           //红色  
		}
		else {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 255, 255);           //白色  
		}
		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;
		// Tell Windows to paint the control itself.  
		*pResult = CDRF_DODEFAULT;

	}
}




void CProcessManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ContextObject != NULL)
	{
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);

		//这个太重要了
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
	}
	CDialog::OnClose();
}

void CProcessManagerDlg::OnServerProcessRefresh()
{
	ShowServerProcessList();
}

void CProcessManagerDlg::OnServerProcessLocation()
{
	int iItem = m_CListCtrl_Server_Process.GetSelectionMark();  //排号
	if (iItem == -1)
	{
		return;
	}
	CHAR ProcessFullPath[MAX_PATH] = { 0 };
	m_CListCtrl_Server_Process.GetItemText(iItem, 2, ProcessFullPath, MAX_PATH);
	CString v1(ProcessFullPath);
	LocationExplorer(v1);
}

VOID CProcessManagerDlg::LocationExplorer(CString ProcessFullPath)
{
	if (!ProcessFullPath.IsEmpty() && PathFileExists(ProcessFullPath))
	{
		CString v1;
		v1.Format("/select,%s", ProcessFullPath);
		ShellExecuteA(NULL, "open", "explorer.exe", v1, NULL, SW_SHOW);
	}
	else
	{
		::MessageBox(m_hWnd, "文件路径错误", NULL, MB_OK | MB_ICONWARNING);
	}
}

void CProcessManagerDlg::OnNMRClickListServerProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_SERVER_PROCESS);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //设置当前窗口
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);

	*pResult = 0;
}

void CProcessManagerDlg::OnNMRClickListClientProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_CLIENT_PROCESS);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //设置当前窗口
	CMenu* SubMenu = Menu.GetSubMenu(0);
	SubMenu->EnableMenuItem(1, MF_REMOVE);
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);

	*pResult = 0;
}

void CProcessManagerDlg::OnClientProcessRefresh()
{
	// TODO: 在此添加命令处理程序代码
	GetClientProcessList();
}

void CProcessManagerDlg::OnClientProcessRing3Kill()
{
	CListCtrl* v1 = &m_CListCtrl_Client_Process;
	LPBYTE BufferData = (LPBYTE)LocalAlloc(LPTR, 1 + (v1->GetSelectedCount() * 4));
	//加入结束进程的数据头
	BufferData[0] = CLIENT_PROCESS_RING3_KILL_REQUIRE;
	//显示警告信息
	char* Tips = "警告: 终止进程会导致不希望发生的结果，\n"
		"包括数据丢失和系统不稳定。在被终止前，\n"
		"进程将没有机会保存其状态和数据。";
	CString v2;
	if (v1->GetSelectedCount() > 1)
	{
		v2.Format("%s确实\n想终止这%d项进程吗?", Tips, v1->GetSelectedCount());
	}
	else
	{
		v2.Format("%s确实\n想终止该项进程吗?", Tips);
	}
	if (::MessageBox(m_hWnd, v2, "进程结束警告", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	DWORD	Offset = 1;
	POSITION Pos = v1->GetFirstSelectedItemPosition();
	//得到要结束哪个进程
	while (Pos)
	{
		int	Item = v1->GetNextSelectedItem(Pos);
		DWORD ProcessID = v1->GetItemData(Item);
		memcpy(BufferData + Offset, &ProcessID, sizeof(DWORD));  //sdkfj101112
		Offset += sizeof(DWORD);
	}
	//发送数据到被控端在被控端中查找COMMAND_KILLPROCESS这个数据头
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, LocalSize(BufferData));
	LocalFree(BufferData);
}

void CProcessManagerDlg::OnServerProcessRing3Kill()
{
	CListCtrl* v1 = &m_CListCtrl_Server_Process;
	PULONG ProcessIDArray = (PULONG)LocalAlloc(LPTR, (v1->GetSelectedCount() * 4));
	//显示警告信息
	char* Tips = "警告: 终止进程会导致不希望发生的结果，\n"
		"包括数据丢失和系统不稳定。在被终止前，\n"
		"进程将没有机会保存其状态和数据。";
	CString v2;
	if (v1->GetSelectedCount() > 1)
	{
		v2.Format("%s确实\n想终止这%d项进程吗?", Tips, v1->GetSelectedCount());
	}
	else
	{
		v2.Format("%s确实\n想终止该项进程吗?", Tips);
	}
	if (::MessageBox(m_hWnd, v2, "进程结束警告", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	DWORD	Offset = 0;
	POSITION Pos = v1->GetFirstSelectedItemPosition();
	//得到要结束哪个进程
	while (Pos)
	{
		int	Item = v1->GetNextSelectedItem(Pos);
		DWORD ProcessID = v1->GetItemData(Item);
		memcpy(ProcessIDArray + Offset, &ProcessID, sizeof(DWORD));  //sdkfj101112
		Offset += sizeof(DWORD);
	}
	//发送数据到被控端在被控端中查找COMMAND_KILLPROCESS这个数据头
	m_Ring3KillProcess(ProcessIDArray, v1->GetSelectedCount());
	LocalFree(ProcessIDArray);

	ShowServerProcessList();
}

void CProcessManagerDlg::OnServerProcessModule()
{
	// TODO: 在此添加命令处理程序代码
	int Item = m_CListCtrl_Server_Process.GetSelectionMark();
	if (Item == -1)
	{
		return;
	}
	ULONG ProcessID = (ULONG)m_CListCtrl_Server_Process.GetItemData(Item);   //获得隐藏数据

	if (OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessID) == NULL)
	{
		ShowErrorMessage("Error", "OpenProcess()");
		return;
	}

	CProcessModuleManagerDlg Dlg(this, ProcessID, PROCESS_MANAGER::SERVER_MODE); //进程模块管理对话框
	Dlg.DoModal();
}
