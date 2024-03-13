#pragma once
#include <windows.h>
#include <mmeapi.h>
#include <MMSYSTEM.H>
#include <MMReg.h>
#pragma comment(lib,"Winmm.lib")

class CAudio
{
public:
	CAudio();
	~CAudio();
	BOOL PlayRecordData(LPBYTE BufferData, DWORD BufferLength);
	BOOL OnInitWaveOut();
	LPBYTE GetRecordData(LPDWORD BufferLength);
	BOOL OnInitWaveIn();
	static DWORD WINAPI WaveInProcedure(LPVOID ParameterData);   //���͵����ض�
private:
	BOOL   m_IsWaveInUsed;
	BOOL   m_IsWaveOutUsed;
	HANDLE m_WaveInThreadHandle;
	//��Ƶ��׼
	GSM610WAVEFORMAT m_GSM610WaveFormat;
	ULONG     m_BufferLength;       //����U�̵Ĵ�С
	LPWAVEHDR m_InAudioHeader[2];   //����U��ͷ��Ϣ
	LPBYTE    m_InAudioData[2];     //����������U��


	LPWAVEHDR m_OutAudioHeader[2];   //����ͷ
	LPBYTE    m_OutAudioData[2];     //��������   ��������������

	HWAVEIN   m_WaveInHandle;        //¼���豸��� 
	ULONG     m_WaveInIndex;

	HWAVEOUT  m_WaveOutHandle;
	ULONG     m_WaveOutIndex;

	HANDLE	  m_EventHandle1;
	HANDLE	  m_EventHandle2;       //�����¼�
};

