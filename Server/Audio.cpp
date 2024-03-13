#include "stdafx.h"
#include "Audio.h"


CAudio::CAudio()
{
	m_IsWaveInUsed = FALSE;
	m_IsWaveOutUsed = FALSE;
	m_WaveInIndex = 0;
	m_WaveOutIndex = 0;
	memset(&m_GSM610WaveFormat, 0, sizeof(GSM610WAVEFORMAT));

	m_GSM610WaveFormat.wfx.wFormatTag = WAVE_FORMAT_GSM610;
	m_GSM610WaveFormat.wfx.nChannels = 1;
	m_GSM610WaveFormat.wfx.nSamplesPerSec = 8000;
	m_GSM610WaveFormat.wfx.nAvgBytesPerSec = 1625;
	m_GSM610WaveFormat.wfx.nBlockAlign = 65;
	m_GSM610WaveFormat.wfx.wBitsPerSample = 0;
	m_GSM610WaveFormat.wfx.cbSize = 2;
	m_GSM610WaveFormat.wSamplesPerBlock = 320;


	m_BufferLength = 1000;

	//创建两个事件
	m_EventHandle1 = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_EventHandle2 = CreateEvent(NULL, TRUE, FALSE, NULL);

	int i = 0;
	for (i = 0; i < 2; i++)
	{
		m_InAudioData[i] = new BYTE[m_BufferLength];
		m_InAudioHeader[i] = new WAVEHDR;


		m_OutAudioData[i] = new BYTE[m_BufferLength];
		m_OutAudioHeader[i] = new WAVEHDR;
	}
}


CAudio::~CAudio()
{
	if (m_IsWaveInUsed)
	{
		waveInStop(m_WaveInHandle);
		waveInReset(m_WaveInHandle);
		for (int i = 0; i < 2; i++)
			waveInUnprepareHeader(m_WaveInHandle, m_InAudioHeader[i], sizeof(WAVEHDR));

		waveInClose(m_WaveInHandle);

		TerminateThread(m_WaveInThreadProcedure, -1);  //为了安全
	}

	for (int i = 0; i < 2; i++)
	{
		delete[] m_InAudioData[i];
		m_InAudioData[i] = NULL;
		delete[] m_InAudioHeader[i];
		m_InAudioHeader[i] = NULL;
	}

	CloseHandle(m_EventHandle1);
	CloseHandle(m_EventHandle2);
	CloseHandle(m_WaveInThreadProcedure);   //线程句柄


	if (m_IsWaveOutUsed)
	{
		waveOutReset(m_WaveOutHandle);
		for (int i = 0; i < 2; i++)
			waveOutUnprepareHeader(m_WaveOutHandle, m_InAudioHeader[i], sizeof(WAVEHDR));
		waveOutClose(m_WaveOutHandle);
	}

	for (int i = 0; i < 2; i++)
	{
		delete[] m_OutAudioData[i];
		m_OutAudioData[i] = NULL;
		delete[] m_OutAudioHeader[i];
		m_OutAudioHeader[i] = NULL;
	}
}
BOOL CAudio::PlayRecordData(LPBYTE BufferData, DWORD BufferLength)
{
	if (!m_IsWaveOutUsed && !OnInitWaveOut())  //1 音频格式   2 播音设备
	{
		return FALSE;
	}
	for (int i = 0; i < BufferLength; i += m_BufferLength)
	{
		memcpy(m_OutAudioData[m_WaveOutIndex], BufferData, m_BufferLength);
		waveOutWrite(m_WaveOutHandle, m_OutAudioHeader[m_WaveOutIndex], sizeof(WAVEHDR));
		m_WaveOutIndex = 1 - m_WaveOutIndex;
	}
	return TRUE;
}
BOOL CAudio::OnInitWaveOut()
{
	if (!waveOutGetNumDevs())
		return FALSE;
	int i;
	for (i = 0; i < 2; i++)
		memset(m_OutAudioData[i], 0, m_BufferLength);  //声音数据

	MMRESULT	MMResult;
	MMResult = waveOutOpen(&m_WaveOutHandle, (WORD)WAVE_MAPPER, &(m_GSM610WaveFormat.wfx), (LONG)0, (LONG)0, CALLBACK_NULL);
	if (MMResult != MMSYSERR_NOERROR)
		return FALSE;

	for (i = 0; i < 2; i++)
	{
		m_OutAudioHeader[i]->lpData = (LPSTR)m_OutAudioData[i];
		m_OutAudioHeader[i]->dwBufferLength = m_BufferLength;
		m_OutAudioHeader[i]->dwFlags = 0;
		m_OutAudioHeader[i]->dwLoops = 0;
		waveOutPrepareHeader(m_WaveOutHandle, m_OutAudioHeader[i], sizeof(WAVEHDR));
	}

	m_IsWaveOutUsed = TRUE;
	return TRUE;
}

LPBYTE CAudio::GetRecordData(LPDWORD BufferLength)
{

	if (m_IsWaveInUsed == FALSE && OnInitWaveIn() == FALSE)
	{
		return NULL;
	}
	if (BufferLength == NULL)
	{
		return NULL;
	}

	SetEvent(m_EventHandle1);
	WaitForSingleObject(m_EventHandle2, INFINITE);
	*BufferLength = m_BufferLength;
	return	m_InAudioData[m_WaveInIndex];              //返出真正数据
}
BOOL CAudio::OnInitWaveIn()
{
	MMRESULT	MMResult;
	DWORD		ThreadID = 0;

	m_WaveInThreadHandle = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)WaveInProcedure, (LPVOID)this,
		CREATE_SUSPENDED, &ThreadID);



	//打开录音设备COM  1 指定声音规格  2 支持通过线程回调 换缓冲
	MMResult = waveInOpen(&m_WaveInHandle, (WORD)WAVE_MAPPER,
		&(m_GSM610WaveFormat.wfx), (LONG)ThreadID, (LONG)0, CALLBACK_THREAD);



	if (MMResult != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	//录音设备 需要的两个缓冲
	for (int i = 0; i < 2; i++)
	{
		m_InAudioHeader[i]->lpData = (LPSTR)m_InAudioData[i];
		m_InAudioHeader[i]->dwBufferLength = m_BufferLength;
		m_InAudioHeader[i]->dwFlags = 0;
		m_InAudioHeader[i]->dwLoops = 0;
		waveInPrepareHeader(m_WaveInHandle, m_InAudioHeader[i], sizeof(WAVEHDR));
	}
	waveInAddBuffer(m_WaveInHandle, m_InAudioHeader[m_WaveInIndex], sizeof(WAVEHDR));
	ResumeThread(m_WaveInThreadHandle);
	waveInStart(m_WaveInHandle);

	m_IsWaveInUsed = TRUE;

	return true;
}
DWORD WINAPI CAudio::WaveInProcedure(LPVOID ParameterData)
{
	CAudio* This = (CAudio*)ParameterData;

	MSG	Msg;


	while (GetMessage(&Msg, NULL, 0, 0))
	{
		if (Msg.message == MM_WIM_DATA)
		{

			SetEvent(This->m_EventHandle2);
			WaitForSingleObject(This->m_EventHandle1, INFINITE);

			Sleep(1);
			This->m_WaveInIndex = 1 - This->m_WaveInIndex;


			//更新缓冲 
			MMRESULT MMResult = waveInAddBuffer(This->m_WaveInHandle,
				This->m_InAudioHeader[This->m_WaveInIndex], sizeof(WAVEHDR));
			if (MMResult != MMSYSERR_NOERROR)
				return -1;

		}

		if (Msg.message == MM_WIM_CLOSE)
		{
			break;
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}
