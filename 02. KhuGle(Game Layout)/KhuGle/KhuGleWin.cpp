//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <cmath>
#include <cstdio>
#include <iostream>

#pragma warning(disable:4996)

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

CKhuGleWin *CKhuGleWin::m_pWinApplication = 0;

void KhuGleWinInit(CKhuGleWin *pApplication)
{
	CKhuGleWin::m_pWinApplication = pApplication;

	WinMain(0, 0, 0, 0);
}

CKhuGleWin::CKhuGleWin(int nW, int nH)
{
	m_pScene = nullptr;
	m_Gravity = CKgVector2D(0., 0.);
	m_AirResistance = CKgVector2D(0., 0.);

	m_nW = nW;
	m_nH = nH;
	m_bViewFps = false;

	for(int i = 0 ; i < 256 ; ++i)
		m_bKeyPressed[i] = false;

	for(int i = 0 ; i < 3 ; ++i)
		m_bMousePressed[i] = false;
}

CKhuGleWin::~CKhuGleWin()
{
	if(m_pScene)
		delete m_pScene;
}

LRESULT CALLBACK CKhuGleWin::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pWinApplication->WndProcInstanceMember(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK CKhuGleWin::WndProcInstanceMember(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int width, height;
	HDC hdc;
	HBRUSH hBrushGray;
	RECT rt;

	hBrushGray = (HBRUSH)GetStockObject(GRAY_BRUSH); 

	double AspectOrg, AspectWin;

	switch (message)
	{
	case WM_CREATE: 
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_CLOSE:  
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		height = HIWORD(lParam);
		width = LOWORD(lParam);

		AspectOrg = (double)m_nW/(double)m_nH;
		AspectWin = (double)width/(double)height;

		m_nDesOffsetX = 0;
		m_nDesOffsetY = 0;
		m_nViewW = width;
		m_nViewH = height;

		if(AspectWin > AspectOrg)
		{
			m_nDesOffsetX = (int)((AspectWin-AspectOrg)*height/2.);
			m_nViewW = (int)(height*AspectOrg);
		}
		else
		{
			m_nDesOffsetY = (int)((1./AspectWin-1./AspectOrg)*width/2.);
			m_nViewH = (int)(width/AspectOrg); 
		}
		break;

	case WM_LBUTTONDOWN:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[0] = true;
		break;

	case WM_LBUTTONUP:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[0] = false;
		break;

	case WM_MBUTTONDOWN:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[1] = true;
		break;

	case WM_MBUTTONUP:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[1] = false;
		break;

	case WM_RBUTTONDOWN:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[2] = true;
		break;

	case WM_RBUTTONUP:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		m_bMousePressed[2] = false;
		break;

	case WM_MOUSEMOVE:
		m_MousePosX = (LOWORD(lParam)-m_nDesOffsetX)*m_nW/m_nViewW; 
		m_MousePosY = (HIWORD(lParam)-m_nDesOffsetY)*m_nH/m_nViewH;
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F11:
			Fullscreen();
			break;

		case VK_F12:
			ToggleFpsView();
			break;

		case VK_LEFT:
			break;

		case VK_RIGHT:
			break;
		}	
		if(wParam >= 0 && wParam < 256)
			m_bKeyPressed[wParam] = true;
		break;

	case WM_KEYUP:
		if(wParam >= 0 && wParam < 256)
			m_bKeyPressed[wParam] = false;
		break;

	case WM_CHAR:
		switch (wParam) 
		{
		case 'a':
			break;
		}
		break;

	case WM_ERASEBKGND:
		hdc = (HDC) wParam; 
		GetClientRect(hwnd, &rt); 
		SetMapMode(hdc, MM_ANISOTROPIC); 
		SetWindowExtEx(hdc, 100, 100, NULL); 
		SetViewportExtEx(hdc, rt.right, rt.bottom, NULL); 
		FillRect(hdc, &rt, hBrushGray); 

		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}

void CKhuGleWin::Fullscreen()
{
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if(dwStyle & WS_OVERLAPPEDWINDOW) 
	{
		m_wpPrev.length = sizeof(WINDOWPLACEMENT);
		MONITORINFO mi = {sizeof(MONITORINFO)};
		if (GetWindowPlacement(m_hWnd, &m_wpPrev) &&
			GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi)) 
		{
			SetWindowLong(m_hWnd, GWL_STYLE,
				dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(m_hWnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else 
	{
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(m_hWnd, &m_wpPrev);
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

void CKhuGleWin::GetFps()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_TimeCountEnd);
	m_ElapsedTime = (double)(m_TimeCountEnd - m_TimeCountStart)/(double)m_TimeCountFreq; 
	m_TimeCountStart = m_TimeCountEnd;
	m_Fps = 1./m_ElapsedTime;
}

void CKhuGleWin::Update()
{
	RECT Rect;
	GetClientRect(m_hWnd, &Rect);

	InvalidateRect(m_pWinApplication->m_hWnd, &Rect, false);

	char strFps[200];

	sprintf(strFps, "FPS: %7.3lf, Elapsed time: %lf", m_Fps, m_ElapsedTime);
	if(m_bViewFps)
	{
		std::cout << strFps << std::endl;
	}
}

void CKhuGleWin::OnPaint()
{
	RECT Rect;
	GetClientRect(m_hWnd, &Rect);

	int nW = Rect.right-Rect.left;
	int nH = Rect.bottom-Rect.top;

	if(nW <= 0 || nH <= 0)
	{
		return;
	}

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	HDC hDC, hCompDC;

	hDC = GetDC(m_hWnd);
	hCompDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmap;

	hBitmap = CreateCompatibleBitmap(hDC, nW, nH);

	SelectObject(hCompDC, hBitmap);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = m_nW;
	bmiHeader.biHeight = m_nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = (m_nW*3+3)/4*4  * m_nH;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	unsigned char *Image2D24 = new unsigned char [bmiHeader.biSizeImage];
	int x, y, Offset;

	for(y = 0 ; y < m_nH ; y++)
	{
		Offset = (m_nW*3+3)/4*4 * (m_nH-y-1);
		for(x = 0 ; x < m_nW ; x++)
		{
			int Offset2 = Offset+x*3;

			if(x < 0 || x >= m_pScene->m_nW || y < 0 || y >= m_pScene->m_nH)
			{
				Image2D24[Offset2++] = 0;
				Image2D24[Offset2++] = 0;
				Image2D24[Offset2] = 0;
			}
			else
			{
				Image2D24[Offset2++] = m_pScene->m_ImageB[y][x];
				Image2D24[Offset2++] = m_pScene->m_ImageG[y][x];
				Image2D24[Offset2] = m_pScene->m_ImageR[y][x];
			}
		}
	}

	SetStretchBltMode(hCompDC, HALFTONE);

	StretchDIBits(hDC,
		m_nDesOffsetX, m_nDesOffsetY, 
		m_nViewW, m_nViewH,
		0,0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		Image2D24,
		(LPBITMAPINFO)&bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);

	delete [] Image2D24;

	DeleteObject(hBitmap);

	DeleteDC(hCompDC);
	ReleaseDC(m_hWnd, hDC);

	EndPaint(m_hWnd, &ps);
}

void CKhuGleWin::DrawSceneTextPos(const char *Text, CKgPoint ptPos)
{
	int nTextHeight = 25;

	HDC hDC;
	HDC hCompDC;
	HBITMAP hBitmap;

	hDC = GetDC(NULL);
	hCompDC = CreateCompatibleDC(hDC);

	hBitmap = CreateCompatibleBitmap(hDC, nTextHeight*strlen(Text), nTextHeight+10);
	SelectObject(hCompDC, hBitmap);

	HFONT hFont;

	hFont = CreateFontA(nTextHeight, 0, 0, 0, 
		FW_NORMAL, 
		0, 0, 0, ANSI_CHARSET, 0, 0, 0, FF_MODERN, 
		"Arial");

	SelectObject(hCompDC, hFont);

	SetTextColor(hCompDC, RGB(0, 0, 0));
	SetBkMode(hCompDC, TRANSPARENT);

	RECT Rt;
	Rt.left = 0;
	Rt.top = 0;
	DrawText(hCompDC, Text, strlen(Text), &Rt, DT_CALCRECT | DT_LEFT);

	BITMAPINFO bi;
	int nW = Rt.right-Rt.left+1;
	int nH = Rt.bottom-Rt.top+1;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nW;
	bi.bmiHeader.biHeight = nH;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	unsigned char *Image = new unsigned char[bi.bmiHeader.biSizeImage];

	for(int y = 0 ; y < nH ; y++)
		for(int x = 0 ; x < nW ; x++)
		{
			if(x+ptPos.X >= m_pScene->m_nW || y+ptPos.Y >= m_pScene->m_nH) break;

			int pos = (nW*3+3)/4*4*(nH-y-1) + x*3;
			Image[pos+2] = m_pScene->m_ImageR[y+ptPos.Y][x+ptPos.X];
			Image[pos+1] = m_pScene->m_ImageG[y+ptPos.Y][x+ptPos.X];
			Image[pos] = m_pScene->m_ImageB[y+ptPos.Y][x+ptPos.X];
		}

	SetStretchBltMode(hCompDC, HALFTONE);

	StretchDIBits(hCompDC,
		0, 0,
		bi.bmiHeader.biWidth,
		bi.bmiHeader.biHeight,
		0,0,
		bi.bmiHeader.biWidth,
		bi.bmiHeader.biHeight,
		Image,
		(LPBITMAPINFO)&bi.bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);


	DrawText(hCompDC, Text, strlen(Text), &Rt, DT_LEFT);

	GetDIBits(hCompDC, hBitmap, 0, nH, Image, &bi, DIB_RGB_COLORS);

	for(int y = 0 ; y < nH ; y++)
		for(int x = 0 ; x < nW ; x++)
		{
			if(x+ptPos.X >= m_pScene->m_nW || y+ptPos.Y >= m_pScene->m_nH) break;

			int pos = (nW*3+3)/4*4*(nH-y-1) + x*3;
			m_pScene->m_ImageR[y+ptPos.Y][x+ptPos.X] = Image[pos+2];
			m_pScene->m_ImageG[y+ptPos.Y][x+ptPos.X] = Image[pos+1];
			m_pScene->m_ImageB[y+ptPos.Y][x+ptPos.X] = Image[pos];
		}

	delete [] Image;

	DeleteObject(hBitmap);
	DeleteObject(hFont);

	DeleteDC(hCompDC);
	ReleaseDC(NULL, hDC);
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	if(!CKhuGleWin::m_pWinApplication) return -1;

	WNDCLASSEX windowClass; 
	MSG		msg;  
	DWORD   dwExStyle;  
	DWORD   dwStyle;    
	RECT    windowRect;

	int width = CKhuGleWin::m_pWinApplication->m_nW;
	int height = CKhuGleWin::m_pWinApplication->m_nH;

	windowRect.left = (long)0;             
	windowRect.right = (long)width;  
	windowRect.top = (long)0;        
	windowRect.bottom = (long)height;

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = CKhuGleWin::m_pWinApplication->WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "WinClass";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass))	return 0;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; 
	dwStyle = WS_OVERLAPPEDWINDOW;


	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	CKhuGleWin::m_pWinApplication->m_hWnd = CreateWindowEx(NULL, "WinClass", 
		"Ai and Data via Game", 
		dwStyle |
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		0, 0,                  
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);    

	if (!CKhuGleWin::m_pWinApplication->m_hWnd)
	{
		return 0;
	}

	ShowWindow(CKhuGleWin::m_pWinApplication->m_hWnd, SW_SHOW);
	UpdateWindow(CKhuGleWin::m_pWinApplication->m_hWnd);  

	QueryPerformanceFrequency((LARGE_INTEGER*)&CKhuGleWin::m_pWinApplication->m_TimeCountFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&CKhuGleWin::m_pWinApplication->m_TimeCountStart);
	
	while(1)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) 
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			CKhuGleWin::m_pWinApplication->GetFps();
			CKhuGleWin::m_pWinApplication->Update();
		}
	}

	delete CKhuGleWin::m_pWinApplication;

	_CrtDumpMemoryLeaks();
	UnregisterClass("WinClass", windowClass.hInstance);

	return msg.wParam;
}

void CKhuGleWin::ToggleFpsView()
{
	m_bViewFps = !m_bViewFps;
}