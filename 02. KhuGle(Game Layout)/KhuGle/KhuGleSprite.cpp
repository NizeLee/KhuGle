//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuGleSprite.h"

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

CKhuGleSprite::CKhuGleSprite(int nType, int nCollisionType, CKgLine lnLine, KgColor24 fgColor, bool bFill, int nSliceOrWidth)
{
	m_nType = nType;
	m_nCollisionType = nCollisionType;
	m_fgColor = fgColor;
	m_bFill = bFill;
	m_nSlice = nSliceOrWidth;
	m_nWidth = nSliceOrWidth;

	if(m_nType == GP_STYPE_LINE)
		m_lnLine = lnLine;
	else 
		m_rtBoundBox = CKgRect(lnLine.Start.X, lnLine.Start.Y, lnLine.End.X, lnLine.End.Y);

	m_Center.x = (lnLine.Start.X + lnLine.End.X)/2.;
	m_Center.y = (lnLine.Start.Y + lnLine.End.Y)/2.;

	m_Velocity = CKgVector2D(0., 0.);
	m_Radius = std::max(fabs((lnLine.Start.X - lnLine.End.X)/2.), fabs((lnLine.Start.Y - lnLine.End.Y)/2.));

	m_Mass = m_Radius*m_Radius;
}

CKhuGleSprite::~CKhuGleSprite()
{
}

void CKhuGleSprite::DrawLine(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x0, int y0, int x1, int y1, KgColor24 Color24)
{
	::DrawLine(R, nW, nH, x0, y0, x1, y1, KgGetRed(Color24));
	::DrawLine(G, nW, nH, x0, y0, x1, y1, KgGetGreen(Color24));
	::DrawLine(B, nW, nH, x0, y0, x1, y1, KgGetBlue(Color24));
}

void CKhuGleSprite::Render()
{
	if(!m_Parent) return;

	CKhuGleLayer *Parent = (CKhuGleLayer *)m_Parent;

	if(m_nType == GP_STYPE_LINE)
	{
		CKgVector2D PosVec = CKgVector2D(m_lnLine.Start) - CKgVector2D(m_lnLine.End);
		CKgVector2D Normal1 = CKgVector2D(PosVec.y, -PosVec.x);
		CKgVector2D Normal2 = CKgVector2D(-PosVec.y, PosVec.x);

		Normal1.Normalize();
		Normal2.Normalize();

		Normal1 = Normal1 * (m_nWidth/2.);
		Normal2 = Normal2 * (m_nWidth/2.);

		DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
			Parent->m_nW, Parent->m_nH, 
			(int)(m_lnLine.Start.X+Normal1.x), (int)(m_lnLine.Start.Y+Normal1.y), (int)(m_lnLine.End.X+Normal1.x), (int)(m_lnLine.End.Y+Normal1.y), m_fgColor);

//		DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
//			Parent->m_nW, Parent->m_nH, 
//			m_lnLine.Start.X, m_lnLine.Start.Y, m_lnLine.End.X, m_lnLine.End.Y, KG_COLOR_24_RGB(255, 255, 255));

		DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
			Parent->m_nW, Parent->m_nH, 
			(int)(m_lnLine.Start.X+Normal2.x), (int)(m_lnLine.Start.Y+Normal2.y), (int)(m_lnLine.End.X+Normal2.x), (int)(m_lnLine.End.Y+Normal2.y), m_fgColor);
	}
	else if(!m_bFill)
	{
		if(m_nType == GP_STYPE_RECT)
		{
			DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
				Parent->m_nW, Parent->m_nH, 
				m_rtBoundBox.Left, m_rtBoundBox.Top, m_rtBoundBox.Right, m_rtBoundBox.Top, m_fgColor);
			DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
				Parent->m_nW, Parent->m_nH, 
				m_rtBoundBox.Right, m_rtBoundBox.Top, m_rtBoundBox.Right, m_rtBoundBox.Bottom, m_fgColor);
			DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
				Parent->m_nW, Parent->m_nH, 
				m_rtBoundBox.Right, m_rtBoundBox.Bottom, m_rtBoundBox.Left, m_rtBoundBox.Bottom, m_fgColor);
			DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
				Parent->m_nW, Parent->m_nH, 
				m_rtBoundBox.Left, m_rtBoundBox.Bottom, m_rtBoundBox.Left, m_rtBoundBox.Top, m_fgColor);
		}
		else
		{
			double RX = (m_rtBoundBox.Right - m_rtBoundBox.Left) / 2.;
			double RY = (m_rtBoundBox.Bottom - m_rtBoundBox.Top) / 2.;
			double CX = (m_rtBoundBox.Right + m_rtBoundBox.Left) / 2.;
			double CY = (m_rtBoundBox.Bottom + m_rtBoundBox.Top) / 2.;

			int i;
			for(i = 0 ; i < m_nSlice ; i++)
			{
				double theta1 = 2.*Pi/m_nSlice*i;
				double theta2 = 2.*Pi/m_nSlice*(i+1);

				DrawLine(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
					Parent->m_nW, Parent->m_nH, 
					(int)(CX + cos(theta1)*RX), (int)(CY + sin(theta1)*RY), 
					(int)(CX + cos(theta2)*RX), (int)(CY + sin(theta2)*RY), m_fgColor);
			}
		}
	}
	else
	{
		if(m_nType == GP_STYPE_RECT)
		{
			CKgRect interRect = CKgRect(0, 0, Parent->m_nW-1, Parent->m_nH-1);
			interRect.Intersect(m_rtBoundBox);

			if(interRect.IsRect())
			{
				for(int y = interRect.Top ; y <= interRect.Bottom ; y++)
					for(int x = interRect.Left ; x <= interRect.Right ; x++)
					{
						Parent->m_ImageR[y][x] = KgGetRed(m_fgColor);
						Parent->m_ImageG[y][x] = KgGetGreen(m_fgColor);
						Parent->m_ImageB[y][x] = KgGetBlue(m_fgColor);
					}
			}
		}
		else
		{
			double RX = (m_rtBoundBox.Right - m_rtBoundBox.Left) / 2.;
			double RY = (m_rtBoundBox.Bottom - m_rtBoundBox.Top) / 2.;
			double CX = (m_rtBoundBox.Right + m_rtBoundBox.Left) / 2.;
			double CY = (m_rtBoundBox.Bottom + m_rtBoundBox.Top) / 2.;

			CKgRect interRect = CKgRect(0, 0, Parent->m_nW-1, Parent->m_nH-1);
			interRect.Intersect(m_rtBoundBox);

			if(interRect.IsRect())
			{
				for(int y = interRect.Top ; y <= interRect.Bottom ; y++)
					for(int x = interRect.Left ; x <= interRect.Right ; x++)
					{
						if((x-CX)*(x-CX)/(RX*RX) + (y-CY)*(y-CY)/(RY*RY) <= 1)
						{
							Parent->m_ImageR[y][x] = KgGetRed(m_fgColor);
							Parent->m_ImageG[y][x] = KgGetGreen(m_fgColor);
							Parent->m_ImageB[y][x] = KgGetBlue(m_fgColor);
						}
					}
			}
		}
	}
}

void CKhuGleSprite::MoveBy(double OffsetX, double OffsetY)
{
	m_Center.x += OffsetX;
	m_Center.y += OffsetY;

	Move();
}

void CKhuGleSprite::MoveTo(double X, double Y)
{
	m_Center.x = X;
	m_Center.y = Y;

	Move();
}

void CKhuGleSprite::Move()
{
	if(m_nType == GP_STYPE_LINE)
	{
		int nW = m_lnLine.End.X - m_lnLine.Start.X;
		int nH = m_lnLine.End.Y - m_lnLine.Start.Y;

		m_lnLine.Start.X = (int)m_Center.x - nW/2;
		m_lnLine.Start.Y = (int)m_Center.y - nH/2;

		m_lnLine.End.X = m_lnLine.Start.X + nW;
		m_lnLine.End.Y = m_lnLine.Start.Y + nH;
	}
	else
	{
		int nW = m_rtBoundBox.Width();
		int nH = m_rtBoundBox.Height();

		m_rtBoundBox.Left = (int)m_Center.x - nW/2;
		m_rtBoundBox.Top = (int)m_Center.y - nH/2;
		m_rtBoundBox.Right = m_rtBoundBox.Left + nW;;
		m_rtBoundBox.Bottom = m_rtBoundBox.Top + nH;
	}
}
