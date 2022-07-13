//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuGleLayer.h"
#include "KhuGleScene.h"

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

CKhuGleScene::CKhuGleScene(int nW, int nH, KgColor24 bgColor)
{
	m_bInit = false;

	SetBackgroundImage(nW, nH, bgColor);
}

CKhuGleScene::~CKhuGleScene()
{
	ResetBackgroundImage();
}

void CKhuGleScene::SetBackgroundImage(int nW, int nH, KgColor24 bgColor)
{
	if(m_bInit) ResetBackgroundImage();

	m_nW = nW;
	m_nH = nH;

	m_bgColor = bgColor;

	m_ImageR = cmatrix(m_nH, m_nW);
	m_ImageG = cmatrix(m_nH, m_nW);
	m_ImageB = cmatrix(m_nH, m_nW);

	int x, y;
	for(y = 0 ; y < m_nH ; y++)
		for(x = 0 ; x < m_nW ; x++)
		{
			m_ImageR[y][x] = KgGetRed(bgColor);
			m_ImageG[y][x] = KgGetGreen(bgColor);
			m_ImageB[y][x] = KgGetBlue(bgColor);
		}

	m_bInit = true;
}

void CKhuGleScene::ResetBackgroundImage()
{
	if(m_bInit)
	{
		free_cmatrix(m_ImageR, m_nH, m_nW);
		free_cmatrix(m_ImageG, m_nH, m_nW);
		free_cmatrix(m_ImageB, m_nH, m_nW);

		m_bInit = false;
	}
}

void CKhuGleScene::SetBgColor(KgColor24 bgColor)
{
	m_bgColor = bgColor;
}

void CKhuGleScene::Render()
{
	int y;
	for(y = 0 ; y < m_nH ; y++)
	{
		memset(m_ImageR[y], KgGetRed(m_bgColor), m_nW);
		memset(m_ImageG[y], KgGetGreen(m_bgColor), m_nW);
		memset(m_ImageB[y], KgGetBlue(m_bgColor), m_nW);
	}

	for(auto &Child : m_Children)
	{
		CKhuGleLayer *Layer = (CKhuGleLayer *)Child;

		Layer->Render();

		for(int y = 0 ; y < Layer->m_nH ; ++y)
		{
			if(y+Layer->m_ptPos.Y >= m_nH) break;

			int nLen = std::min(Layer->m_nW, m_nW-Layer->m_ptPos.X);

			if(nLen <= 0) continue;
			memcpy(m_ImageR[y+Layer->m_ptPos.Y] + Layer->m_ptPos.X, Layer->m_ImageR[y], nLen);
			memcpy(m_ImageG[y+Layer->m_ptPos.Y] + Layer->m_ptPos.X, Layer->m_ImageG[y], nLen);
			memcpy(m_ImageB[y+Layer->m_ptPos.Y] + Layer->m_ptPos.X, Layer->m_ImageB[y], nLen);
		}
	}
}

