//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include "KhuGleBase.h"
#include "KhuGleLayer.h"
#include "KhuGleScene.h"
#include "KhuGleComponent.h"

#define GP_STYPE_LINE			0
#define GP_STYPE_RECT			1
#define GP_STYPE_ELLIPSE		2

#define GP_CTYPE_STATIC			0
#define GP_CTYPE_DYNAMIC		1
#define GP_CTYPE_KINEMATIC		2

class CKhuGleSprite : public CKhuGleComponent
{
public:
	int m_nType;
	int m_nCollisionType;
	CKgLine m_lnLine;
	CKgRect m_rtBoundBox;
	KgColor24 m_fgColor;
	bool m_bFill;
	int m_nWidth;
	int m_nSlice;

	CKgVector2D m_Center, m_Velocity, m_Acceleration;
	double m_Radius;
	double m_Mass;

	CKhuGleSprite() {}
	CKhuGleSprite(int nType, int nCollisionType, CKgLine lnLine, KgColor24 fgColor, bool bFill, int nSliceOrWidth = 100);
	~CKhuGleSprite();

	static void DrawLine(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x0, int y0, int x1, int y1, KgColor24 Color24);

	virtual void Render();
	void MoveBy(double OffsetX, double OffsetY);
	void MoveTo(double X, double Y);
	void Move();
};

