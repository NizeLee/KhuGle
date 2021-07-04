//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include "KhuGleSignal.h"
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

class CKhuGleGraphLayer : public CKhuGleLayer
{
public:
	int m_nCurrentCnt;
	std::vector<std::vector<double>> m_Data;
	std::vector<double> m_MaxData;
	int m_nDataTotal;
	double m_TrainAccuacy, m_TrainLoss;

	CKhuGleGraphLayer(int nW, int nH, KgColor24 bgColor, int nDataTotal, CKgPoint ptPos = CKgPoint(0, 0));
	void SetMaxData(int nIndex, double Value);
	void DrawBackgroundImage();
};

CKhuGleGraphLayer::CKhuGleGraphLayer(int nW, int nH, KgColor24 bgColor, int nDataTotal, CKgPoint ptPos)
	: m_MaxData(nDataTotal), m_Data(nDataTotal), CKhuGleLayer(nW, nH, bgColor, ptPos)
{
	m_bgColor = bgColor;
	m_nCurrentCnt = 0;
	m_nDataTotal = nDataTotal;

	m_TrainAccuacy = 0.;
	m_TrainLoss = 2.5;
}

void CKhuGleGraphLayer::SetMaxData(int nIndex, double Value)
{
	m_MaxData[nIndex] = Value;
}

void CKhuGleGraphLayer::DrawBackgroundImage()
{
	for(int y = 0 ; y < m_nH ; y++)
		for(int x = 0 ; x < m_nW ; x++)
		{
			m_ImageBgR[y][x] = KgGetRed(m_bgColor);
			m_ImageBgG[y][x] = KgGetGreen(m_bgColor);
			m_ImageBgB[y][x] = KgGetBlue(m_bgColor);
		}

	int xx0, yy0, xx1, yy1;
	for(int k = 0 ; k < m_nDataTotal ; ++k)
	{
		KgColor24 Color = KG_COLOR_24_RGB(k%2*255, k/2%2*255, k/4%2*255);
		for(int i = 0 ; i < m_nCurrentCnt ; ++i) {
			xx1 = i*m_nW/m_nCurrentCnt;
			yy1 = (int)(m_nH - m_Data[k][i]*m_nH/m_MaxData[k] - 1);
			if(yy1 < 0) yy1 = 0;
			if(yy1 >= m_nH) yy1 = m_nH-1;

			if(i > 0) {
				CKhuGleSprite::DrawLine(m_ImageBgR, m_ImageBgG, m_ImageBgB, m_nW, m_nH, 
					xx0, yy0, xx1, yy1, Color);
			}

			xx0 = xx1;
			yy0 = yy1;
		}
	}
}

class CKhuGleRocLayer : public CKhuGleLayer
{
public:
	std::vector<std::pair<int, double>> m_Data;
	std::vector<std::pair<double, double>> m_Positive;

	CKhuGleRocLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0))
		: CKhuGleLayer(nW, nH, bgColor, ptPos)
	{
		MakeData();
		ComputePositives();
		DrawBackgroundImage();
	}
	void MakeData();
	void ComputePositives();
	void DrawBackgroundImage();
};

void CKhuGleRocLayer::MakeData()
{
	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::poisson_distribution<int> poisson_dist(85);

	m_Data.clear();

	for(int i = 0 ; i < 2000 ; ++i)
	{
		double score = poisson_dist(generator)/100.;
		if(score < 0) continue;
		if(score > 1) continue;

		if(rand()%10 > 3)
		{
			if(score > 0.85) m_Data.push_back({1, score});
			else if(score > 0.15) m_Data.push_back({rand()%10<5?0:1, score});
			else m_Data.push_back({0, score});
		}
		else
			m_Data.push_back({rand()%10 < 5?0:1, score});
	}
}

void CKhuGleRocLayer::ComputePositives()
{
	m_Positive.clear();

	for(int nThreshold = 0 ; nThreshold <= 100 ; nThreshold += 1)
	{
		double TP = 0, FP = 0;
		int nPositiveCnt = 0;
		for(auto &Data : m_Data)
		{
			if(Data.second >= nThreshold/100.)
			{
				if(Data.first == 1)
					TP++;
				else
					FP++;
			}

			if(Data.first == 1)
				nPositiveCnt++;
		}

		TP /= nPositiveCnt;
		FP /= (m_Data.size()-nPositiveCnt);

		m_Positive.push_back({TP, FP});
	}
}

void CKhuGleRocLayer::DrawBackgroundImage()
{
	for(int y = 0 ; y < m_nH ; y++)
		for(int x = 0 ; x < m_nW ; x++)
		{
			m_ImageBgR[y][x] = KgGetRed(m_bgColor);
			m_ImageBgG[y][x] = KgGetGreen(m_bgColor);
			m_ImageBgB[y][x] = KgGetBlue(m_bgColor);
		}

	int xx0, yy0, xx1, yy1;
	bool bFirst = true;
	for(auto &Positive : m_Positive)
	{
		xx1 = (int)(Positive.second * (m_nW-1));
		yy1 = m_nH-(int)(Positive.first * (m_nH-1))-1;

		if(!bFirst)
			CKhuGleSprite::DrawLine(m_ImageBgR, m_ImageBgG, m_ImageBgB, m_nW, m_nH, 
				xx0, yy0, xx1, yy1, KG_COLOR_24_RGB(0, 255, 0));

		bFirst = false;
		
		xx0 = xx1;
		yy0 = yy1;
	}
}

class CPerformance : public CKhuGleWin
{
public:
	CKhuGleGraphLayer *m_pTrainGraphLayer;
	CKhuGleRocLayer *m_pRocLayer;
		
	CPerformance(int nW, int nH);
	void Update();
};

CPerformance::CPerformance(int nW, int nH) : CKhuGleWin(nW, nH) 
{
	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pTrainGraphLayer = new CKhuGleGraphLayer(600, 200, KG_COLOR_24_RGB(150, 150, 200), 2, CKgPoint(20, 30));
	m_pTrainGraphLayer->SetMaxData(0, 100.);
	m_pTrainGraphLayer->SetMaxData(1, 2.5);
	m_pScene->AddChild(m_pTrainGraphLayer);

	m_pRocLayer = new CKhuGleRocLayer(200, 200, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 250));
	m_pScene->AddChild(m_pRocLayer);
}

void CPerformance::Update()
{
	if(m_bKeyPressed['N'])
	{
		m_pRocLayer->MakeData();
		m_pRocLayer->ComputePositives();
		m_pRocLayer->DrawBackgroundImage();

		m_bKeyPressed['N'] = false;
	}

	unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	std::uniform_real_distribution<double> uniform_dist1(-0.43, 0.5);
	std::uniform_real_distribution<double> uniform_dist2(-0.7, 0.5);
	double alpha = uniform_dist1(generator);
	double beta = uniform_dist2(generator);

	m_pTrainGraphLayer->m_TrainAccuacy = 0.99*m_pTrainGraphLayer->m_TrainAccuacy + 0.01*(alpha+m_pTrainGraphLayer->m_TrainAccuacy);
	m_pTrainGraphLayer->m_TrainLoss = 0.99*m_pTrainGraphLayer->m_TrainLoss + 0.01*(beta+m_pTrainGraphLayer->m_TrainLoss);
	if(m_pTrainGraphLayer->m_TrainAccuacy > 1) m_pTrainGraphLayer->m_TrainAccuacy = 1;
	if(m_pTrainGraphLayer->m_TrainLoss < 0) m_pTrainGraphLayer->m_TrainLoss = 0;

	m_pTrainGraphLayer->m_Data[0].push_back(100*m_pTrainGraphLayer->m_TrainAccuacy);
	m_pTrainGraphLayer->m_Data[1].push_back(m_pTrainGraphLayer->m_TrainLoss);
	m_pTrainGraphLayer->m_nCurrentCnt++;
	m_pTrainGraphLayer->DrawBackgroundImage();

	m_pScene->Render();
	DrawSceneTextPos("Performance Evaluation", CKgPoint(0, 0));

	CKhuGleWin::Update();
}

int main()
{
	CPerformance *pPerformance = new CPerformance(640, 480);

	KhuGleWinInit(pPerformance);

	return 0;
}