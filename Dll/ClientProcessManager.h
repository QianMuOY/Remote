#pragma once
#include "Manager.h"
#include <vector>
#include "Common.h"

#pragma pack(1) 
struct _PROCESS_INFORMATION_
{
    ULONG32 ProcessID;
    char   ImageNameData[MAX_PATH];
    char   ProcessFullPathData[MAX_PATH];
};

typedef SIZE_T(*LPFN_SEENUMPROCESSLIST)(vector<_PROCESS_INFORMATION_>&); 

class CClientProcessManager :
    public CManager
{
public:
    CClientProcessManager(_CIOCPClient* ClientObject);
    ~CClientProcessManager();
    BOOL SendClientProcessList();
    virtual void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);

private:
    HMODULE m_ProcessModuleBase;
    LPFN_SEENUMPROCESSLIST m_SeEnumProcessList = NULL;
    f4 m_Ring3KillProcess = NULL;
};

