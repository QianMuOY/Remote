#pragma once
#include "stdafx.h"
#include "Common.h"
BOOL WcharToChar(char** DestinationString, wchar_t* SourceString, SIZE_T SourceStringLength)
{
	if (SeIsValidPointer(SourceString, SourceStringLength) == FALSE)
	{
		return FALSE;
	}
	SIZE_T DestinationStringLength = 0;
	DestinationStringLength = wcstombs(NULL, SourceString, 0);
	*DestinationString = new char[DestinationStringLength + 1];
	if (*DestinationString == NULL)
	{
		return FALSE;
	}
	memset(*DestinationString, 0, DestinationStringLength + 1);
	if (wcstombs(*DestinationString, SourceString, DestinationStringLength + 1) == -1)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SeIsValidPointer(PVOID BaseData, ULONG BufferLength)
{
	if ((BaseData == NULL) || (BaseData == (PVOID)~0))
		return FALSE;

	SE_ASSERT(!IsBadReadPtr(BaseData, BufferLength), L"IsBadReadPtr(BaseData, BufferLength)");

	return TRUE;
}
void SeAssert(BOOL Assert, LPCWSTR Message)
{
	if (Assert)
		return;

#ifdef _DEBUG
	DebugBreak();
#endif
	FatalAppExitW(0, Message);
}

VOID ShowErrorMessage(CHAR* Class, CHAR* Message)
{
	MessageBox(NULL, Message, Class, 0);
}

BOOL EnableSeDebugPrivilege(IN const CHAR*  PriviledgeName, BOOL IsEnable)
{
	// 打开权限令牌

	HANDLE  ProcessHandle = GetCurrentProcess();
	HANDLE  TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges = { 0 };
	if (!OpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return FALSE;
	}
	LUID			 v1;
	if (!LookupPrivilegeValue(NULL, PriviledgeName, &v1))		// 通过权限名称查找uID
	{
		CloseHandle(TokenHandle);
		TokenHandle = NULL;
		return FALSE;
	}

	TokenPrivileges.PrivilegeCount = 1;		// 要提升的权限个数
	TokenPrivileges.Privileges[0].Attributes = IsEnable == TRUE ? SE_PRIVILEGE_ENABLED : 0;    // 动态数组，数组大小根据Count的数目
	TokenPrivileges.Privileges[0].Luid = v1;


	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges,
		sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{

		CloseHandle(TokenHandle);
		TokenHandle = NULL;
		return FALSE;
	}
	CloseHandle(TokenHandle);
	TokenHandle = NULL;
	return TRUE;
}