#pragma once
#include "Manager.h"
#include "Common.h"
class CClientCmdManager : public CManager
{
public:
	CClientCmdManager(_CIOCPClient* ClientObject);
	~CClientCmdManager();
	void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	static  DWORD WINAPI  ReceiveProcedure(LPVOID ParameterData);

public:
	HANDLE  m_ReadHandle1;
	HANDLE  m_WriteHandle1;
	HANDLE  m_ReadHandle2;
	HANDLE  m_WriteHandle2;
	HANDLE  m_ThreadHandle;
	BOOL    m_IsLoop;
	HANDLE  m_CmdProcessHandle;
	HANDLE  m_CmdThreadHandle;
};
