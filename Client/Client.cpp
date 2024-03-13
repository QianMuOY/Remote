#include "Client.h"

struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;            //为了服务端可以找到可以写入IP和端口的Flag
	char  ServerIPAddress[20];  //IP地址
	USHORT   ServerConnectionPort;    //连接的端口
}__ServerConnectInfo = { 0x99999999,"192.168.235.1",8888 };  //本机测试/虚拟机测试

//{ "192.168.3.11",8888 }//"192.168.235.1"



int main()
{
	printf("ServerIPAddress:%s\r\n", __ServerConnectInfo.ServerIPAddress);  //"127.0.0.1"  打印IP信息
	printf("ServerConnectionPort:%d\r\n", __ServerConnectInfo.ServerConnectionPort);  //"127.0.0.1"  打印端口信息

	HMODULE  ModuleHandle = (HMODULE)LoadLibrary("Dll.dll");
	if (ModuleHandle == NULL)
	{
		return 0;
	}

	//获取DLL模块中的一个导出函数
	LPFN_CLIENTRUN ClientRun =
		(LPFN_CLIENTRUN)GetProcAddress(ModuleHandle, "ClientRun");  //回调函数


	if (ClientRun == NULL)
	{
		FreeLibrary(ModuleHandle);
		return 0;
	}

	else
	{
		ClientRun(__ServerConnectInfo.ServerIPAddress, __ServerConnectInfo.ServerConnectionPort);
	}



	printf("Input AnyKey To Exit\r\n");
	getchar();
	FreeLibrary(ModuleHandle);

	return 0;

}