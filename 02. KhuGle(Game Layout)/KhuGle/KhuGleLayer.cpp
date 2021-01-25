//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleLayer.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <cstring>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

CKhuGleLayer::CKhuGleLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos)
{
	m_bInit = false;
	m_ptPos = ptPos;

	SetBackgroundImage(nW, nH, bgColor);
}

CKhuGleLayer::~CKhuGleLayer()
{
	ResetBackgroundImage();
}

void CKhuGleLayer::SetBackgroundImage(int nW, int nH, KgColor24 bgColor)
{
	if(m_bInit) ResetBackgroundImage();

	m_nW = nW;
	m_nH = nH;

	m_bgColor = bgColor;

	m_ImageR = cmatrix(m_nH, m_nW);
	m_ImageG = cmatrix(m_nH, m_nW);
	m_ImageB = cmatrix(m_nH, m_nW);

	m_ImageBgR = cmatrix(m_nH, m_nW);
	m_ImageBgG = cmatrix(m_nH, m_nW);
	m_ImageBgB = cmatrix(m_nH, m_nW);

	int x, y;
	for(y = 0 ; y < m_nH ; y++)
		for(x = 0 ; x < m_nW ; x++)
		{
			m_ImageBgR[y][x] = KgGetRed(bgColor);
			m_ImageBgG[y][x] = KgGetGreen(bgColor);
			m_ImageBgB[y][x] = KgGetBlue(bgColor);
		}

	m_bInit = true;
}

void CKhuGleLayer::ResetBackgroundImage()
{
	if(m_bInit)
	{
		free_cmatrix(m_ImageR, m_nH, m_nW);
		free_cmatrix(m_ImageG, m_nH, m_nW);
		free_cmatrix(m_ImageB, m_nH, m_nW);

		free_cmatrix(m_ImageBgR, m_nH, m_nW);
		free_cmatrix(m_ImageBgG, m_nH, m_nW);
		free_cmatrix(m_ImageBgB, m_nH, m_nW);

		m_bInit = false;
	}
}

void CKhuGleLayer::SetBgColor(KgColor24 bgColor)
{
	m_bgColor = bgColor;
}

void CKhuGleLayer::Render()
{
	int y;
	for(y = 0 ; y < m_nH ; y++)
	{
		memcpy(m_ImageR[y], m_ImageBgR[y], m_nW);
		memcpy(m_ImageG[y], m_ImageBgG[y], m_nW);
		memcpy(m_ImageB[y], m_ImageBgB[y], m_nW);
	}

	for(auto &Child : m_Children)
		Child->Render();
}
