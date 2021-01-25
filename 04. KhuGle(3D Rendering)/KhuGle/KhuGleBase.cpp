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

CKgVector2D &CKgVector2D::operator+= (CKgVector2D v) 
{
	*this = *this + v;
	return *this;
}

CKgVector2D operator*(double s, CKgVector2D v)
{
	return CKgVector2D(s*v.x, s*v.y);
}

double CKgVector3D::abs(CKgVector3D v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void CKgVector3D::Normalize()
{
	double Magnitude = abs(*this);

	if(Magnitude == 0) return;

	x /= Magnitude;
	y /= Magnitude;
	z /= Magnitude;
}

double CKgVector3D::Dot(CKgVector3D v)
{
	return x*v.x + y*v.y + z*v.z;
}

CKgVector3D CKgVector3D::Cross(CKgVector3D v)
{
	CKgVector3D NewV;

	NewV.x = y*v.z - z*v.y;
	NewV.y = z*v.x - x*v.z;
	NewV.z = x*v.y - y*v.x;

	return NewV;
}

CKgVector3D CKgVector3D::operator+ (CKgVector3D v)
{
	return CKgVector3D(x+v.x, y+v.y, z+v.z);
}

CKgVector3D CKgVector3D::operator- (CKgVector3D v)
{
	return CKgVector3D(x-v.x, y-v.y, z-v.z);
}

CKgVector3D CKgVector3D::operator- ()
{
	return CKgVector3D(-x, -y, -z);
}

CKgVector3D &CKgVector3D::operator+= (CKgVector3D v)
{
	*this = *this + v;
	return *this;
}

CKgVector3D operator*(double s, CKgVector3D v)
{
	return CKgVector3D(s*v.x, s*v.y, s*v.z);
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

// LU decomposition
bool ludcmp(double **a, int nN, int *indx, double *d)
{
	int i, imax, j, k;
	double big, dum, sum, temp;
	double *vv = new double[nN];
	const double TinyValue = 1.0e-20;

	*d = 1.0;
	for(i = 0; i < nN; i++)
	{
		big = 0.0;
		for (j = 0; j < nN; j++)
			if ((temp = fabs(a[i][j])) > big)
				big = temp;

		if (big == 0.0)
		{
			delete[] vv;
			return false; // Singular
		}

		vv[i] = 1.0 / big;
	}

	for(j = 0; j < nN; j++)
	{
		for(i = 0; i < j; i++)
		{
			sum = a[i][j];
			for(k = 0; k < i; k++)
				sum -= a[i][k] * a[k][j];

			a[i][j] = sum;
		}

		big = 0.0;
		for(i = j; i < nN; i++)
		{
			sum = a[i][j];
			for(k = 0; k < j; k++)
				sum -= a[i][k] * a[k][j];

			a[i][j] = sum;
			if((dum = vv[i] * fabs(sum)) >= big)
			{
				big = dum;
				imax = i;
			}
		}

		if(j != imax)
		{
			for (k = 0; k < nN; k++)
			{
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}

		indx[j] = imax;
		if(a[j][j] == 0.0) a[j][j] = TinyValue;

		if(j != nN - 1)
		{
			dum = 1.0 / (a[j][j]);
			for (i = j + 1; i < nN; i++)
				a[i][j] *= dum;
		}
	}
	delete[] vv;

	return true;
}

void lubksb(double **a, int nN, int *indx, double *b)
{
	int i, ii = -1, ip, j;
	double sum;

	for(i = 0; i < nN; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];

		if(ii >= 0)
		{
			for(j = ii; j <= i - 1; j++)
			{
				sum -= a[i][j] * b[j];
			}
		}
		else if(sum)
			ii = i;

		b[i] = sum;
	}
	for(i = nN - 1; i >= 0; i--)
	{
		sum = b[i];
		for (j = i + 1; j < nN; j++)
			sum -= a[i][j] * b[j];
		b[i] = sum / a[i][i];
	}
}

// Orginal code: NUMERICAL RECIPES IN C 
bool InverseMatrix(double  **a, double **y, int nN)
{
	double **CopyA = dmatrix(nN, nN);

	double *col = new double[nN];
	int *indx = new int[nN];
	double d;

	for(int r = 0; r < nN; r++)
		for(int c = 0; c < nN; c++)
			CopyA[r][c] = a[r][c];

	if(!ludcmp(CopyA, nN, indx, &d))
	{
		free_dmatrix(CopyA, nN, nN);

		delete[] indx;
		delete[] col;

		return false;
	}

	for(int j = 0; j < nN; j++) {
		for(int i = 0; i < nN; i++)
			col[i] = 0.0;
		col[j] = 1.0;

		lubksb(CopyA, nN, indx, col);

		for(int i = 0; i < nN; i++)
			y[i][j] = col[i];
	}

	free_dmatrix(CopyA, nN, nN);

	delete[] indx;
	delete[] col;

	return true;
}