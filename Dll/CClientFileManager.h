#pragma once
#include "Manager.h"

typedef struct
{
	DWORD	FileSizeHigh;
	DWORD	FileSizeLow;
}FILE_SIZE;

class CClientFileManager :
    public CManager
{
public:
	CClientFileManager(_CIOCPClient* ClientObject);
	~CClientFileManager();

	void HandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	ULONG SendClientVolumeList();
	int SendClientFileList(PBYTE DirectoryFullPath);
	VOID CreateReceivedFileInformation(LPBYTE BufferData);
	BOOL MakeSureDirectoryPathExists(char* DirectoryFullPath);
	VOID SetTransferMode(LPBYTE BufferData);
	VOID GetServerFileData();
	VOID WriteReceivedFileData(LPBYTE BufferData, ULONG BufferLength);
private:

	char    m_FileFullPath[MAX_PATH];
	ULONG   m_TransferMode;

};

