#pragma once

#include "_IOCPClient.h"
#include "Common.h"
class CClientInstantManager :
	public CManager
{
public:
	CClientInstantManager(_CIOCPClient* ClientObject);
	~CClientInstantManager();
	virtual void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
};

int CALLBACK DialogProcedure(HWND DlgHwnd, unsigned int Msg,
	WPARAM wParam, LPARAM lParam);
VOID OnInitDialog(HWND DlgHwnd);
VOID OnTimerDialog(HWND DlgHwnd);

