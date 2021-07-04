//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include <windows.h>
#include "KhuGleBase.h"
#include "KhuGleSprite.h"
#include "KhuGleLayer.h"
#include "KhuGleScene.h"
#include "KhuGleComponent.h"

void PlayWave(short int *Sound, int nSampleRate, int nLen);
void StopWave();
void GetPlaybackPosotion(unsigned long *Rate);

class CKhuGleWin;
void KhuGleWinInit(CKhuGleWin *pApplication);

class CKhuGleWin
{
public:
	HWND m_hWnd;
	int m_nW, m_nH;

	CKgVector2D m_Gravity;
	CKgVector2D m_AirResistance;

	static CKhuGleWin *m_pWinApplication;

	int m_nDesOffsetX, m_nDesOffsetY;
	int m_nViewW, m_nViewH;

	_int64 m_TimeCountFreq, m_TimeCountStart, m_TimeCountEnd;
	double m_Fps, m_ElapsedTime;

	bool m_bKeyPressed[256];
	bool m_bMousePressed[3];
	int m_MousePosX, m_MousePosY;

	WINDOWPLACEMENT m_wpPrev;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProcInstanceMember(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Fullscreen();

	void GetFps();
	virtual void Update();
	void OnPaint();

	void DrawSceneTextPos(char *Text, CKgPoint ptPos);
	void ToggleFpsView();

	CKhuGleWin(int nW, int nH);
	virtual ~CKhuGleWin();
	bool m_bViewFps;

	CKhuGleScene *m_pScene;
};

