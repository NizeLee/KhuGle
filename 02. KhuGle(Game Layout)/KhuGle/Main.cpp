//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <iostream>

class CGameLayout : public CKhuGleWin
{
public:
	CKhuGleLayer *m_pGameLayer;

	CKhuGleSprite *m_pCircle1;
	CKhuGleSprite *m_pCircle2;
	CKhuGleSprite *m_pRect;
	
	CGameLayout(int nW, int nH);
	void Update();

	CKgPoint m_LButtonStart, m_LButtonEnd;
	int m_nLButtonStatus;
};

CGameLayout::CGameLayout(int nW, int nH) : CKhuGleWin(nW, nH) 
{
	m_nLButtonStatus = 0;

	m_Gravity = CKgVector2D(0., 98.);
	m_AirResistance = CKgVector2D(0.1, 0.1);

	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pGameLayer = new CKhuGleLayer(600, 420, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 30));

	m_pScene->AddChild(m_pGameLayer);
	
	m_pCircle1 = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(30, 30), CKgPoint(90, 90)), 
		KG_COLOR_24_RGB(255, 0, 0), true, 100);
	m_pCircle2 = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(70, 70), CKgPoint(130, 130)), 
		KG_COLOR_24_RGB(255, 0, 0), false, 100);
	m_pRect = new CKhuGleSprite(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(300, 350), CKgPoint(450, 250)), 
		KG_COLOR_24_RGB(255, 0, 0), false, 10);

	m_pGameLayer->AddChild(m_pCircle1);
	m_pGameLayer->AddChild(m_pCircle2);
	m_pGameLayer->AddChild(m_pRect);
}

void CGameLayout::Update()
{
	if(m_bMousePressed[0]) {
		if(m_nLButtonStatus == 0)		{
			m_LButtonStart = CKgPoint(m_MousePosX, m_MousePosY);
		}
		m_LButtonEnd = CKgPoint(m_MousePosX, m_MousePosY);
		m_nLButtonStatus = 1;
	}
	else {
		if(m_nLButtonStatus == 1) {
			std::cout << m_LButtonStart.X << "," << m_LButtonStart.Y << std::endl;
			std::cout << m_LButtonEnd.X << "," << m_LButtonEnd.Y << std::endl;

			m_nLButtonStatus = 0;
		}
	}

	if(m_bKeyPressed[VK_LEFT]) m_pCircle1->MoveBy(-1, 0);
	if(m_bKeyPressed[VK_UP]) m_pCircle1->MoveBy(0, -1);
	if(m_bKeyPressed[VK_RIGHT]) m_pCircle1->MoveBy(1, 0);
	if(m_bKeyPressed[VK_DOWN]) m_pCircle1->MoveBy(0, 1);

	m_pScene->Render();
	DrawSceneTextPos("CGame Layout", CKgPoint(0, 0));

	CKhuGleWin::Update();
}

int main()
{
	CGameLayout *pGameLayout = new CGameLayout(640, 480);

	KhuGleWinInit(pGameLayout);

	return 0;
}