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

class CCorrelationLayer : public CKhuGleLayer {
public:
	std::vector<CKhuGleSprite *> m_Point;

	CCorrelationLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0))
		: CKhuGleLayer(nW, nH, bgColor, ptPos) {
		GenerateData(200);
	}
	void GenerateData(int nCnt);
};

void CCorrelationLayer::GenerateData(int nCnt)
{
	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_real_distribution<double> uniform_dist(0, 1);

	for(auto &Child : m_Children)
		delete Child;	
	m_Children.clear();
	m_Point.clear();

	double mean1 = uniform_dist(generator);
	double mean2 = uniform_dist(generator);
	double sigma1 = uniform_dist(generator)/2.;
	double sigma2 = uniform_dist(generator)/2.;
	double rotate = uniform_dist(generator)*Pi;
	std::normal_distribution<double> normal_dist1(mean1, sigma1);
	std::normal_distribution<double> normal_dist2(mean2, sigma2);

	double x, y;

	for(int i = 0 ; i < nCnt ; i++)
	{
		double xx = normal_dist1(generator);
		double yy = normal_dist2(generator);

		x = (xx-mean1)*cos(rotate) - (yy-mean2)*sin(rotate) + mean1;
		y = (xx-mean1)*sin(rotate) + (yy-mean2)*cos(rotate) + mean2;

		x = (x*m_nW - m_nW/2)*0.6 + m_nW/2;
		y = (y*m_nH - m_nH/2)*0.6 + m_nH/2;

		CKhuGleSprite *Point = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint((int)x-2, (int)y-2), CKgPoint((int)x+2, (int)y+2)), 
			KG_COLOR_24_RGB(255, 255, 255), true, 30);

		m_Point.push_back(Point);
		AddChild(Point);
	}
}

class CKhuGleSprite2 : public CKhuGleSprite {
public:
	int m_nClusterIndex;
	CKhuGleSprite2(int nType, int nCollisionType, CKgLine lnLine, KgColor24 fgColor, bool bFill, int nSliceOrWidth = 100, int nClusterIndex = 0)
		: CKhuGleSprite(nType, nCollisionType, lnLine, fgColor, bFill, nSliceOrWidth) 
	{
		m_nClusterIndex = nClusterIndex;
	}
};

class CClusterLayer : public CKhuGleLayer {
public:
	std::vector<CKhuGleSprite2 *> m_Center;
	std::vector<CKhuGleSprite2 *> m_Point;
	int m_nClusterNum, m_nStep;

	CClusterLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0))
		: CKhuGleLayer(nW, nH, bgColor, ptPos) {
		m_nClusterNum = 3;

		GenerateData(m_nClusterNum, 50);

		m_nStep = 0;
	}
	void GenerateData(int nCluster, int nCnt);
};

void CClusterLayer::GenerateData(int nCluster, int nCnt)
{
	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_real_distribution<double> uniform_dist(0, 1);

	for(auto &Child : m_Children)
		delete Child;	
	m_Children.clear();
	m_Center.clear();
	m_Point.clear();

	for(int i = 0 ; i < m_nClusterNum ; ++i)
	{
		CKhuGleSprite2 *Center = new CKhuGleSprite2(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint(m_nW/2-10, m_nH/2-10), CKgPoint(m_nW/2+10, m_nH/2+10)), 
			KG_COLOR_24_RGB(i%2*255, i/2%2*255, i/4%2*255), false, 100);

		m_Center.push_back(Center);
		AddChild(Center);
	}

	for(int k = 0 ; k < nCluster ; ++k)
	{
		double mean1 = uniform_dist(generator);
		double mean2 = uniform_dist(generator);
		double sigma1 = uniform_dist(generator)/10.;
		double sigma2 = uniform_dist(generator)/10.;
		double rotate = uniform_dist(generator)*Pi;
		std::normal_distribution<double> normal_dist1(mean1, sigma1);
		std::normal_distribution<double> normal_dist2(mean2, sigma2);

		double x, y;

		for(int i = 0 ; i < nCnt ; i++)
		{
			double xx = normal_dist1(generator);
			double yy = normal_dist2(generator);

			x = (xx-mean1)*cos(rotate) - (yy-mean2)*sin(rotate) + mean1;
			y = (xx-mean1)*sin(rotate) + (yy-mean2)*cos(rotate) + mean2;

			x = (x*m_nW - m_nW/2)*0.6 + m_nW/2;
			y = (y*m_nH - m_nH/2)*0.6 + m_nH/2;

			CKhuGleSprite2 *Point = new CKhuGleSprite2(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint((int)x-2, (int)y-2), CKgPoint((int)x+2, (int)y+2)), 
				KG_COLOR_24_RGB(255, 255, 255), true, 30);

			m_Point.push_back(Point);
			AddChild(Point);
		}
	}
}

class CCorrelationClustering : public CKhuGleWin {
public:
	CKhuGleScene *m_pCorrelationScene;
	CKhuGleScene *m_pClusteringScene;

	CCorrelationLayer *m_pCorrelationLayer;
	CClusterLayer *m_pClusteringLayer;

	bool m_bCorrelationScene;

	CCorrelationClustering(int nW, int nH);
	virtual ~CCorrelationClustering() {
		m_pScene = nullptr;
		delete m_pCorrelationScene;
		delete m_pClusteringScene;
	}
	void Update();
};

CCorrelationClustering::CCorrelationClustering(int nW, int nH)
	: CKhuGleWin(nW, nH) {
	m_pCorrelationScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));
	m_pClusteringScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));
	
	m_pCorrelationLayer = new CCorrelationLayer(400, 400, KG_COLOR_24_RGB(200, 150, 200), CKgPoint(120, 40));
	m_pCorrelationScene->AddChild(m_pCorrelationLayer);

	m_pClusteringLayer = new CClusterLayer(400, 400, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(120, 40));
	m_pClusteringScene->AddChild(m_pClusteringLayer);

	m_pScene = m_pCorrelationScene;
	m_bCorrelationScene = true;
}

void CCorrelationClustering::Update()
{
	if(m_bKeyPressed['M'])
	{
		m_bCorrelationScene = !m_bCorrelationScene;
		if(m_bCorrelationScene)
			m_pScene = m_pCorrelationScene;
		else
			m_pScene = m_pClusteringScene;
		m_bKeyPressed['M'] = false;
	}

	if(m_bKeyPressed['S'])
	{
		if(m_bCorrelationScene)
		{
			std::vector<std::pair<double, double>> Data;
			for(auto Point : m_pCorrelationLayer->m_Point)
				Data.push_back({Point->m_Center.x, Point->m_Center.y});
			
			double pcc = GetPearsonCoefficient(Data);
			std::cout << pcc << std::endl;
		}
		else
		{
			if(m_pClusteringLayer->m_nStep == 0)
			{
				for(auto &Center : m_pClusteringLayer->m_Center)
					Center->MoveTo((double)rand()/RAND_MAX*m_pClusteringLayer->m_nW, (double)rand()/RAND_MAX*m_pClusteringLayer->m_nH);
			}
			else
			{
				std::vector<int> ClusterCnt;
				std::vector<std::pair<double, double>> NewCenter;

				for(auto &Center : m_pClusteringLayer->m_Center)
				{
					NewCenter.push_back({0., 0.});
					ClusterCnt.push_back(0);
				}
			
				for(auto &Point : m_pClusteringLayer->m_Point)
				{
					int Index = Point->m_nClusterIndex;
				
					NewCenter[Index].first += Point->m_Center.x;
					NewCenter[Index].second += Point->m_Center.y;

					ClusterCnt[Index]++;
				}
				for(int k = 0 ; k < m_pClusteringLayer->m_nClusterNum ; ++k)
				{
					if(ClusterCnt[k] > 0)
						m_pClusteringLayer->m_Center[k]->MoveTo(NewCenter[k].first/ClusterCnt[k], NewCenter[k].second/ClusterCnt[k]);
				}
			}

			for(auto &Point : m_pClusteringLayer->m_Point)
			{
				double MinDist, Dist;
				for(int k = 0 ; k < m_pClusteringLayer->m_nClusterNum ; ++k)
				{
					Dist = sqrt((Point->m_Center.x - m_pClusteringLayer->m_Center[k]->m_Center.x)*(Point->m_Center.x - m_pClusteringLayer->m_Center[k]->m_Center.x) +
						(Point->m_Center.y - m_pClusteringLayer->m_Center[k]->m_Center.y)*(Point->m_Center.y - m_pClusteringLayer->m_Center[k]->m_Center.y));
				
					if(k == 0) {
						Point->m_nClusterIndex = k;
						MinDist = Dist;
					}
					else if(Dist < MinDist)
					{
						Point->m_nClusterIndex = k;
						MinDist = Dist;
					}
				}

				Point->m_fgColor = KG_COLOR_24_RGB(Point->m_nClusterIndex%2*255, Point->m_nClusterIndex/2%2*255, Point->m_nClusterIndex/4%2*255);
			}

			++(m_pClusteringLayer->m_nStep);
		}
		m_bKeyPressed['S'] = false;
	}

	if(m_bKeyPressed['N'])
	{
		if(m_bCorrelationScene)
			m_pCorrelationLayer->GenerateData(200);
		else
		{
			m_pClusteringLayer->GenerateData(m_pClusteringLayer->m_nClusterNum, 50);
			m_pClusteringLayer->m_nStep = 0;
		}

		m_bKeyPressed['N'] = false;
	}

	m_pScene->Render();
	if(m_bCorrelationScene)
		DrawSceneTextPos("Correlation && Clustering (Correlation scene)", CKgPoint(0, 0));
	else
		DrawSceneTextPos("Correlation && Clustering (Clustering scene)", CKgPoint(0, 0));
	
	CKhuGleWin::Update();
}

int main()
{
	CCorrelationClustering *pCorrelationClustering = new CCorrelationClustering(640, 480);
	KhuGleWinInit(pCorrelationClustering);

	return 0;
}