// InstantMessageDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "InstantMessageDlg.h"
#include "afxdialogex.h"


// CInstantMessageDlg 对话框

IMPLEMENT_DYNAMIC(CInstantMessageDlg, CDialog)

CInstantMessageDlg::CInstantMessageDlg(CWnd* pParent, _CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_INSTANT_MESSAGE, pParent)
{
	m_IOCPServer =	IOCPServer;
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CInstantMessageDlg::~CInstantMessageDlg()
{
}

void CInstantMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INSTANT_MESSAGE_MAIN, m_CEdit_Main);
}


BOOL CInstantMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_IconHwnd, FALSE);   //设置红客图标到Dialog
	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CInstantMessageDlg::OnClose()
{
	CancelIo((HANDLE)m_ContextObject->ClientSocket);
	closesocket(m_ContextObject->ClientSocket);
	CDialog::OnClose();
}

BOOL CInstantMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类


	if (pMsg->message == WM_KEYDOWN)
	{
		// 点击ESC
		if (pMsg->wParam == VK_ESCAPE)
		{
			PostMessage(WM_CLOSE);
			return true;
		}

		//如果是可编辑框的回车键 点击回车键 发送当前控件中的数据
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Main.m_hWnd)
		{
			OnClientSending();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CInstantMessageDlg::OnClientSending()  //给客户端发送消息
{
	// TODO: 在此添加控件通知处理程序代码
	//获得Edit控件上的数据
	int BufferLength = m_CEdit_Main.GetWindowTextLength();   //EditBox 上获得数据长度
	if (!BufferLength)
	{
		return;
	}
	CString v1;
	m_CEdit_Main.GetWindowText(v1);            //EditBox 上获得数据

	//IO通信套接字只支持char型数据
	char* BufferData = (char*)VirtualAlloc(NULL, BufferLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memset(BufferData, 0, sizeof(char) * BufferLength);
	sprintf(BufferData, "%s", v1.GetBuffer(0));

	//请求清空控件上的数据
	m_CEdit_Main.SetWindowText(NULL);          //EditBox 上的数据清空

	//调用通信引擎类的发送数据函数
	m_IOCPServer->OnClientPreSending(m_ContextObject,
		(LPBYTE)BufferData, strlen(BufferData));   //将自己内存中的数据发送

	//释放
	if (BufferData != NULL)
	{
		VirtualFree(BufferData, BufferLength, MEM_RELEASE);
	}
}



BEGIN_MESSAGE_MAP(CInstantMessageDlg, CDialog)
END_MESSAGE_MAP()


// CInstantMessageDlg 消息处理程序
