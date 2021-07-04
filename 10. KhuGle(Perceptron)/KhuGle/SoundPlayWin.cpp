//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

HWAVEOUT hPlay;
bool bSoundPlaying = false;
WAVEFORMATEX WaveFormat;
WAVEHDR WaveHdr;

void CALLBACK waveOutProc(HWAVEOUT hWO, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	MMRESULT mmRes;

	switch(uMsg)
	{
	case MM_WOM_DONE:     
		mmRes = waveOutUnprepareHeader(hWO, (LPWAVEHDR)dwParam2, sizeof(WAVEHDR));
		bSoundPlaying = false;
		break;
	}
}

void PlayWave(short int *Sound, int nSampleRate, int nLen)
{
	unsigned long mmReturn = 0;

	if(!bSoundPlaying)
	{

		WaveFormat.wFormatTag = 1;
		WaveFormat.nChannels = 1;
		WaveFormat.nSamplesPerSec = nSampleRate;
		WaveFormat.nAvgBytesPerSec = nSampleRate*2;
		WaveFormat.nBlockAlign = 2;
		WaveFormat.wBitsPerSample = 16;
		WaveFormat.cbSize = sizeof(WAVEFORMATEX);

		MMRESULT mmReturn = waveOutOpen(&hPlay, WAVE_MAPPER, &WaveFormat, 
			(DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
		if(mmReturn)
		{
			return;
		}

		if(!mmReturn)
		{
			bSoundPlaying = true;
		}

		WaveHdr.lpData            = (char *)Sound;
		WaveHdr.dwBufferLength    = nLen*2;
		WaveHdr.dwFlags           = 0;
		WaveHdr.dwLoops           = 0;

		waveOutPrepareHeader(hPlay, &WaveHdr, sizeof(WAVEHDR));
		waveOutWrite(hPlay, &WaveHdr, sizeof(WaveHdr));
	}
}

void StopWave()
{
	if(bSoundPlaying)
	{
		waveOutReset(hPlay);
		waveOutClose(hPlay);
	}
}

void GetPlaybackPosotion(unsigned long *pPosition)
{
	if(!bSoundPlaying) 
	{
		*pPosition = 0;

		return;
	}

	MMTIME pmmt;
	pmmt.wType = TIME_SAMPLES;

	MMRESULT mmReturn = waveOutGetPosition(hPlay, &pmmt, sizeof(MMTIME));
	
	*pPosition = pmmt.u.sample;
	
	if(mmReturn)
	{
		*pPosition = 0;
	}
}