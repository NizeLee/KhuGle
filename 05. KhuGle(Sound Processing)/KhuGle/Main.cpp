//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include "KhuGleSignal.h"
#include <iostream>

class CKhuGleSoundLayer : public CKhuGleLayer
{
public:
	CKhuGleSignal m_Sound;
	int m_nViewType;

	CKhuGleSoundLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0));
	void SetBackgroundImage(int nW, int nH, KgColor24 bgColor);
};

CKhuGleSoundLayer::CKhuGleSoundLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos)
	: CKhuGleLayer(nW, nH, bgColor, ptPos)
{
	m_nViewType = 0;
	m_bgColor = bgColor;
}

void CKhuGleSoundLayer::SetBackgroundImage(int nW, int nH, KgColor24 bgColor)
{
	CKhuGleLayer::SetBackgroundImage(nW, nH, bgColor);

	if(m_nViewType == 0 && m_Sound.m_Samples)
	{
		int xx0, yy0, xx1, yy1;
		for(int i = 0 ; i < m_Sound.m_nSampleLength ; ++i)
		{
			xx1 = i*m_nW/m_Sound.m_nSampleLength;
			yy1 = m_nH-(m_Sound.m_Samples[i]+32768)*m_nH/65536-1;

			if(i > 0)
				CKhuGleSprite::DrawLine(m_ImageBgR, m_ImageBgG, m_ImageBgB, m_nW, m_nH, 
					xx0, yy0, xx1, yy1, KG_COLOR_24_RGB(255, 0, 255));

			xx0 = xx1;
			yy0 = yy1;
		}
	}

	if(m_nViewType == 1 && m_Sound.m_Real && m_Sound.m_Imaginary)
	{
		double Max = 0;
		for(int y = 0 ; y < m_nH ; y++)
			for(int x = 0 ; x < m_nW ; x++)
			{
				int yy = (m_nH-y-1)/2*m_Sound.m_nWindowSize/m_nH;
				int xx = x*m_Sound.m_nFrequencySampleLength/m_nW;

				double Magnitude = sqrt(m_Sound.m_Real[xx][yy]*m_Sound.m_Real[xx][yy] + m_Sound.m_Imaginary[xx][yy]*m_Sound.m_Imaginary[xx][yy]);
				if(Magnitude > Max) Max = Magnitude;
			}

		for(int y = 0 ; y < m_nH ; y++)
			for(int x = 0 ; x < m_nW ; x++)
			{
				int yy = (m_nH-y-1)/2*m_Sound.m_nWindowSize/m_nH;
				int xx = x*m_Sound.m_nFrequencySampleLength/m_nW;
				
				m_ImageBgR[y][x] = (int)(sqrt(m_Sound.m_Real[xx][yy]*m_Sound.m_Real[xx][yy] + m_Sound.m_Imaginary[xx][yy]*m_Sound.m_Imaginary[xx][yy])*255/Max);
				m_ImageBgG[y][x] = m_ImageBgR[y][x];
				m_ImageBgB[y][x] = m_ImageBgR[y][x];
			}
	}

	if(m_nViewType == 2 && m_Sound.m_Real && m_Sound.m_Imaginary)
	{
		double Max = 0, Min = 0;
		for(int y = 0 ; y < m_nH ; y++)
			for(int x = 0 ; x < m_nW ; x++)
			{
				int yy = (m_nH-y-1)/2*m_Sound.m_nWindowSize/m_nH;
				int xx = x*m_Sound.m_nFrequencySampleLength/m_nW;

				double Magnitude = sqrt(m_Sound.m_Real[xx][yy]*m_Sound.m_Real[xx][yy] + m_Sound.m_Imaginary[xx][yy]*m_Sound.m_Imaginary[xx][yy]);
				Magnitude = 10*log10(Magnitude*Magnitude+1.);
				if(x == 0 && y == 0) {
					Min = Magnitude;
					Max = Magnitude;
				}

				if(Magnitude > Max) Max = Magnitude;
				if(Magnitude < Min) Min = Magnitude;
			}

		for(int y = 0 ; y < m_nH ; y++)
			for(int x = 0 ; x < m_nW ; x++)
			{
				int yy = (m_nH-y-1)/2*m_Sound.m_nWindowSize/m_nH;
				int xx = x*m_Sound.m_nFrequencySampleLength/m_nW;

				double Magnitude = sqrt(m_Sound.m_Real[xx][yy]*m_Sound.m_Real[xx][yy] + m_Sound.m_Imaginary[xx][yy]*m_Sound.m_Imaginary[xx][yy]);
				Magnitude = 10*log10(Magnitude*Magnitude+1.);

				m_ImageBgR[y][x] = (int)((Magnitude-Min)*255/(Max-Min));
				m_ImageBgG[y][x] = m_ImageBgR[y][x];
				m_ImageBgB[y][x] = m_ImageBgR[y][x];
			}
	}
}

class CSoundProcessing : public CKhuGleWin
{
public:
	CKhuGleSoundLayer *m_pSoundLayer;
	CKhuGleSprite *m_pSoundLine;

	CSoundProcessing(int nW, int nH, char *SoundPath);
	void Update();
};

CSoundProcessing::CSoundProcessing(int nW, int nH, char *SoundPath) : CKhuGleWin(nW, nH) 
{
	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pSoundLayer = new CKhuGleSoundLayer(600, 200, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 30));
	m_pSoundLayer->m_Sound.ReadWave(SoundPath);
	m_pSoundLayer->SetBackgroundImage(600, 200, m_pSoundLayer->m_bgColor);
	m_pScene->AddChild(m_pSoundLayer);

	m_pSoundLayer->m_Sound.MakeSpectrogram();

	m_pSoundLine = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 0), CKgPoint(0, 199)), 
		KG_COLOR_24_RGB(255, 0, 0), false, 0);
	m_pSoundLayer->AddChild(m_pSoundLine);
}

void CSoundProcessing::Update()
{
	if(m_bKeyPressed['S']) 
	{
		StopWave();
	}

	if(m_bKeyPressed['T'] || m_bKeyPressed['F'] || m_bKeyPressed['L'])
	{
		if(m_bKeyPressed['T']) m_pSoundLayer->m_nViewType = 0;
		if(m_bKeyPressed['F']) m_pSoundLayer->m_nViewType = 1;
		if(m_bKeyPressed['L']) m_pSoundLayer->m_nViewType = 2;

		m_pSoundLayer->SetBackgroundImage(m_pSoundLayer->m_nW, m_pSoundLayer->m_nH, m_pSoundLayer->m_bgColor);
	}

	if(m_bKeyPressed['P'])
		PlayWave(m_pSoundLayer->m_Sound.m_Samples, m_pSoundLayer->m_Sound.m_nSampleRate, m_pSoundLayer->m_Sound.m_nSampleLength);

	unsigned long nPosition;
	GetPlaybackPosotion(&nPosition);
	if(nPosition > 0)
	{
		m_pSoundLine->MoveTo(nPosition*600/m_pSoundLayer->m_Sound.m_nSampleLength, m_pSoundLayer->m_nH/2);
	}

	m_pScene->Render();
	DrawSceneTextPos("Sound Processing", CKgPoint(0, 0));

	CKhuGleWin::Update();
}

int main()
{
	char ExePath[MAX_PATH], SoundPath[MAX_PATH];

	GetModuleFileName(NULL, ExePath, MAX_PATH);

	int i;
	int LastBackSlash = -1;
	int nLen = strlen(ExePath);
	for(i = nLen-1 ; i >= 0 ; i--)
	{
		if(ExePath[i] == '\\') {
			LastBackSlash = i;
			break;
		}
	}

	if(LastBackSlash >= 0)
		ExePath[LastBackSlash] = '\0';

	sprintf(SoundPath, "%s\\%s", ExePath, "ex.wav");

	CSoundProcessing *pSoundProcessing = new CSoundProcessing(640, 480, SoundPath);

	KhuGleWinInit(pSoundProcessing);

	return 0;
}