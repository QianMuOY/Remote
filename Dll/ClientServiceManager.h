#pragma once
#include "Manager.h"
#include "Common.h"

class CClientServiceManager :
	public CManager
{
public:
	CClientServiceManager(_CIOCPClient* IOCPClient);
	~CClientServiceManager();
	VOID SendClientServiceList();
	LPBYTE GetClientServiceList();
	void ConfigClientService(PBYTE BufferData, ULONG BufferLength);
	virtual void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
private:
	SC_HANDLE m_ServiceManagerHandle;
};
