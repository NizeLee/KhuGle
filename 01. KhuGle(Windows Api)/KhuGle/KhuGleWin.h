//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include <windows.h>

class CKhuGleWin;
void KhuGleWinInit(CKhuGleWin *pApplication);

class CKhuGleWin
{
public:
	HWND m_hWnd;
	int m_nW, m_nH;

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

	void ToggleFpsView();

	CKhuGleWin(int nW, int nH);
	virtual ~CKhuGleWin();
	bool m_bViewFps;
};

