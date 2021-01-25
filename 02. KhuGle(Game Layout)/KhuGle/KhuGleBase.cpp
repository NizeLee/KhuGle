//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleBase.h"
#include <cmath>

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

CKgPoint CKgPoint::operator+ (CKgPoint p1) 
{
	return CKgPoint(X+p1.X, Y+p1.Y);
}

CKgPoint &CKgPoint::operator+= (CKgPoint p1) 
{
	*this = *this + p1;
	return *this;
}

bool CKgRect::IsRect() 
{
	if(Width() <= 0) return false;
	if(Height() <= 0) return false;
	return true;
}

int CKgRect::Width()
{
	return Right-Left;
}

int CKgRect::Height()
{
	return Bottom-Top;
}

CKgPoint CKgRect::Center()
{
	return CKgPoint((Left+Right)/2, (Top+Bottom)/2);
}

void CKgRect::Move(int x, int y)
{
	Left += x;
	Top += y;
	Right += x;
	Bottom += y;
}

void CKgRect::Intersect(CKgRect rt)
{
	Left = std::max(Left, rt.Left);
	Top = std::max(Top, rt.Top);
	Right = std::min(Right, rt.Right);
	Bottom = std::min(Bottom, rt.Bottom);
}

void CKgRect::Union(CKgRect rt)
{
	Left = std::min(Left, rt.Left);
	Top = std::min(Top, rt.Top);
	Right = std::max(Right, rt.Right);
	Bottom = std::max(Bottom, rt.Bottom);
}

void CKgRect::Expanded(int e)
{
	Left -= e;
	Top -= e;
	Right += e;
	Bottom += e;
}

double CKgVector2D::abs(CKgVector2D v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

void CKgVector2D::Normalize() {
	double Magnitude = abs(*this);

	if(Magnitude == 0) return;

	x /= Magnitude;
	y /= Magnitude;
}

double CKgVector2D::Dot(CKgVector2D v)
{
	return x*v.x + y*v.y;
}

CKgVector2D CKgVector2D::operator+ (CKgVector2D v) 
{
	return CKgVector2D(x+v.x, y+v.y);
}

CKgVector2D CKgVector2D::operator- (CKgVector2D v) 
{
	return CKgVector2D(x-v.x, y-v.y);
}

CKgVector2D CKgVector2D::operator- () 
{
	return CKgVector2D(-x, -y);
}

CKgVector2D CKgVector2D::operator* (double s) 
{
	return CKgVector2D(s*x, s*y);
}

CKgVector2D &CKgVector2D::operator+= (CKgVector2D v) 
{
	*this = *this + v;
	return *this;
}

unsigned char **cmatrix(int nH, int nW) {
	unsigned char **Temp;

	Temp = new unsigned char *[nH];
	for(int y = 0 ; y < nH ; y++)
		Temp[y] = new unsigned char[nW];

	return Temp;
}

void free_cmatrix(unsigned char **Image, int nH, int nW) {
	for(int y = 0 ; y < nH ; y++)
		delete [] Image[y];

	delete [] Image;
}

double **dmatrix(int nH, int nW) {
	double **Temp;

	Temp = new double *[nH];
	for(int y = 0 ; y < nH ; y++)
		Temp[y] = new double[nW];

	return Temp;
}

void free_dmatrix(double **Image, int nH, int nW) {
	for(int y = 0 ; y < nH ; y++)
		delete [] Image[y];

	delete [] Image;
}

void DrawLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color)
{
	int nDiffX = abs(x0-x1);
	int nDiffY = abs(y0-y1);

	int x, y;
	int nFrom, nTo;

	if(nDiffY == 0 && nDiffX == 0)
	{
		y = y0;
		x = x0;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			ImageGray[y][x] = Color;
	}
	else if(nDiffX == 0)
	{
		x = x0;

		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
	else if(nDiffY == 0)
	{
		y = y0;

		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
	else if(nDiffY > nDiffX)
	{
		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			x = (y-y0)*(x0-x1)/(y0-y1) + x0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
	else
	{
		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			y = (x-x0)*(y0-y1)/(x0-x1) + y0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
}
