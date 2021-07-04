//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <iostream>
#include <random>
#include <chrono>

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

class CLsmLayer : public CKhuGleLayer {
public:
	std::vector<CKhuGleSprite *> m_Point;
	bool m_bQuadricCurve;
	int m_nPointCnt;
	double **m_X, *m_y, *m_w;

	CLsmLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0), int nPointCnt = 100)
		: CKhuGleLayer(nW, nH, bgColor, ptPos) {
		m_X = nullptr;
		m_y = nullptr;
		m_w = nullptr;

		m_bQuadricCurve = true;

		GenerateData(nPointCnt, false);
	}
	virtual ~CLsmLayer() {
		if(m_X) free_dmatrix(m_X, m_nPointCnt, 3);
		if(m_y) delete [] m_y;
		if(m_w) delete [] m_w;
	}
	void GenerateData(int nCnt, bool bExtremeNoise);
};

void CLsmLayer::GenerateData(int nCnt, bool bExtremeNoise)
{
	if(m_X) free_dmatrix(m_X, m_nPointCnt, 3);
	if(m_y) delete [] m_y;
	if(m_w) delete [] m_w;

	m_nPointCnt = nCnt;

	m_X = dmatrix(m_nPointCnt, 3);
	m_y = new double[m_nPointCnt];
	m_w = new double[3];

	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_real_distribution<double> uniform_dist1(0.005, 0.01);
	std::uniform_real_distribution<double> uniform_dist2(m_nW*0.4, m_nW*0.6);
	std::uniform_real_distribution<double> uniform_dist3(m_nH*0.9, m_nH*0.95);
	std::uniform_real_distribution<double> uniform_dist4(m_nW*0.1, m_nW*0.9);
	std::uniform_real_distribution<double> uniform_dist5(0, m_nW*0.1);
	double a = -uniform_dist1(generator);
	double x0 = uniform_dist2(generator);
	double y0 = uniform_dist3(generator);
	double ExtremeNoisePos = uniform_dist4(generator);


	for(auto &Child : m_Children)
		delete Child;	
	m_Children.clear();
	m_Point.clear();

	double x, y, noise;
	double m = (rand()%2?1:-1)*a*100;
	for(int i = 0 ; i < m_nPointCnt ; ++i)
	{
		noise = uniform_dist5(generator)-m_nW*0.05;
		x = uniform_dist4(generator);

		if(m_bQuadricCurve)
		{
			y = a*(x-x0)*(x-x0) + y0 + noise;
		}
		else
		{
			y = m*(x-x0) + y0 + noise;
		}

		if(bExtremeNoise)
		{
			if(x > ExtremeNoisePos-m_nW*0.05 && x < ExtremeNoisePos+m_nW*0.05)
			{
				if(m_bQuadricCurve)
					y = a*(x-x0)*(x-x0) + y0 + (noise-m_nW*0.05)*3;
				else
					y = m*(x-x0) + y0 + (noise-m_nW*0.05)*3;
			}
		}

		m_X[i][0] = x*x;
		m_X[i][1] = x;
		m_X[i][2] = 1;

		m_y[i] = y;

		CKhuGleSprite *Point = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint((int)x-2, (int)y-2), CKgPoint((int)x+2, (int)y+2)), 
			KG_COLOR_24_RGB(255, 200, 255), false, 30);

		m_Point.push_back(Point);
		AddChild(Point);
	}

	SetBackgroundImage(m_nW, m_nH, m_bgColor);
}

class CRegression : public CKhuGleWin {
public:
	CLsmLayer *m_pLsmLayer;

	CRegression(int nW, int nH);
	void Update();
};

CRegression::CRegression(int nW, int nH)
	: CKhuGleWin(nW, nH) {
	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));
	m_pLsmLayer = new CLsmLayer(400, 400, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(120, 40), 200);
	m_pScene->AddChild(m_pLsmLayer);
}

void CRegression::Update()
{
	if(m_bKeyPressed['Q'])
	{
		m_pLsmLayer->m_bQuadricCurve = !m_pLsmLayer->m_bQuadricCurve;

		m_pLsmLayer->GenerateData(200, false);
		m_bKeyPressed['Q'] = false;
	}
	if(m_bKeyPressed['S'])
	{
		LeastSquared(m_pLsmLayer->m_X, m_pLsmLayer->m_w, m_pLsmLayer->m_y, m_pLsmLayer->m_nPointCnt, 3, false, 0);

		int y0;
		for(int x = 0 ; x < m_pLsmLayer->m_nW ; ++x)
		{
			int y = (int)(m_pLsmLayer->m_w[0]*x*x + m_pLsmLayer->m_w[1]*x + m_pLsmLayer->m_w[2]);
			if(x > 0)
			{
				CKhuGleSprite::DrawLine(m_pLsmLayer->m_ImageBgR, m_pLsmLayer->m_ImageBgG, m_pLsmLayer->m_ImageBgB, 
					m_pLsmLayer->m_nW, m_pLsmLayer->m_nH, x-1, y0, x, y, KG_COLOR_24_RGB(255, 0, 0));
			}
			y0 = y;
		}

		LeastSquared(m_pLsmLayer->m_X, m_pLsmLayer->m_w, m_pLsmLayer->m_y, m_pLsmLayer->m_nPointCnt, 3, true, 0.9);

		for(int x = 0 ; x < m_pLsmLayer->m_nW ; ++x)
		{
			int y = (int)(m_pLsmLayer->m_w[0]*x*x + m_pLsmLayer->m_w[1]*x + m_pLsmLayer->m_w[2]);
			if(x > 0)
			{
				CKhuGleSprite::DrawLine(m_pLsmLayer->m_ImageBgR, m_pLsmLayer->m_ImageBgG, m_pLsmLayer->m_ImageBgB, 
					m_pLsmLayer->m_nW, m_pLsmLayer->m_nH, x-1, y0, x, y, KG_COLOR_24_RGB(255, 255, 0));
			}
			y0 = y;
		}

		m_bKeyPressed['S'] = false;
	}
	if(m_bKeyPressed['N'] || m_bKeyPressed['M'])
	{
		if(m_bKeyPressed['M'])
			m_pLsmLayer->GenerateData(200, true);
		else
			m_pLsmLayer->GenerateData(200, false);

		m_bKeyPressed['N'] = false;
		m_bKeyPressed['M'] = false;
	}

	m_pScene->Render();

	if(m_pLsmLayer->m_bQuadricCurve)
		DrawSceneTextPos("Regression: quadric curve", CKgPoint(0, 0));
	else 
		DrawSceneTextPos("Regression: line", CKgPoint(0, 0));
	
	CKhuGleWin::Update();
}

int main()
{
	CRegression *pRegression = new CRegression(640, 480);
	KhuGleWinInit(pRegression);

	return 0;
}