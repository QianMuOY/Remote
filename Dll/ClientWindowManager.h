#pragma once
#include "Manager.h"
#include "Common.h"



class CClientWindowManager :
    public CManager
{
public:
	CClientWindowManager(_CIOCPClient* IOCPClient);
	~CClientWindowManager();
	void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	void SendClientWindowList();
	static BOOL CALLBACK EnumWindowProcedure(HWND Hwnd, LPARAM ParameterData);
private:
	HMODULE m_WindowModuleBase = NULL;
	f2 m_GetWindowList;
	f3 m_SetWindowStatus;
	f3 m_PostWindowMessage;
};

