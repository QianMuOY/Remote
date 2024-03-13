#pragma once
#include <Windows.h>
#include <MMSYSTEM.H>
#include <MMReg.h>
#pragma comment(lib,"Winmm.lib")
class CAudio
{
public:
	CAudio();
	~CAudio();
	BOOL CAudio::PlayRecordData(LPBYTE BufferData, DWORD BufferLength);
	BOOL CAudio::OnInitWaveOut();
	LPBYTE CAudio::GetRecordData(LPDWORD BufferLength);
	BOOL CAudio::OnInitWaveIn();
	static DWORD WINAPI WaveInProcedure(LPVOID ParameterData);   //���͵��ͻ���
private:
	GSM610WAVEFORMAT m_GSM610WaveFormat;
	BOOL      m_IsWaveInUsed;
	ULONG     m_BufferLength;
	LPWAVEHDR m_InAudioHeader[2];   //����ͷ
	LPBYTE    m_InAudioData[2];     //��������   ��������������
	HANDLE	  m_EventHandle1;
	HANDLE	  m_EventHandle2;       //�����¼�
	HWAVEIN   m_WaveInHandle;       //�豸���	
	DWORD     m_WaveInIndex;
	HANDLE    m_WaveInThreadProcedure;
	HANDLE    m_WaveInThreadHandle;

	HWAVEOUT m_WaveOutHandle;
	BOOL     m_IsWaveOutUsed;
	DWORD    m_WaveOutIndex;
	LPWAVEHDR m_OutAudioHeader[2];   //����ͷ
	LPBYTE    m_OutAudioData[2];     //��������   ��������������


};

