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
	static DWORD WINAPI WaveInProcedure(LPVOID ParameterData);   //发送到客户端
private:
	GSM610WAVEFORMAT m_GSM610WaveFormat;
	BOOL      m_IsWaveInUsed;
	ULONG     m_BufferLength;
	LPWAVEHDR m_InAudioHeader[2];   //两个头
	LPBYTE    m_InAudioData[2];     //两个数据   保持声音的连续
	HANDLE	  m_EventHandle1;
	HANDLE	  m_EventHandle2;       //两个事件
	HWAVEIN   m_WaveInHandle;       //设备句柄	
	DWORD     m_WaveInIndex;
	HANDLE    m_WaveInThreadProcedure;
	HANDLE    m_WaveInThreadHandle;

	HWAVEOUT m_WaveOutHandle;
	BOOL     m_IsWaveOutUsed;
	DWORD    m_WaveOutIndex;
	LPWAVEHDR m_OutAudioHeader[2];   //两个头
	LPBYTE    m_OutAudioData[2];     //两个数据   保持声音的连续


};

