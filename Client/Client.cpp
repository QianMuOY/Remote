#include "Client.h"

struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;            //Ϊ�˷���˿����ҵ�����д��IP�Ͷ˿ڵ�Flag
	char  ServerIPAddress[20];  //IP��ַ
	USHORT   ServerConnectionPort;    //���ӵĶ˿�
}__ServerConnectInfo = { 0x99999999,"192.168.235.1",8888 };  //��������/���������

//{ "192.168.3.11",8888 }//"192.168.235.1"



int main()
{
	printf("ServerIPAddress:%s\r\n", __ServerConnectInfo.ServerIPAddress);  //"127.0.0.1"  ��ӡIP��Ϣ
	printf("ServerConnectionPort:%d\r\n", __ServerConnectInfo.ServerConnectionPort);  //"127.0.0.1"  ��ӡ�˿���Ϣ

	HMODULE  ModuleHandle = (HMODULE)LoadLibrary("Dll.dll");
	if (ModuleHandle == NULL)
	{
		return 0;
	}

	//��ȡDLLģ���е�һ����������
	LPFN_CLIENTRUN ClientRun =
		(LPFN_CLIENTRUN)GetProcAddress(ModuleHandle, "ClientRun");  //�ص�����


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