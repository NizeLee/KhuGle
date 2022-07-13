//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include <algorithm>
#include <cmath>

#define Pi	3.14159

typedef unsigned long KgColor24;
#define KG_COLOR_24_RGB(R, G, B)		((unsigned long)(((unsigned char)(R)|((unsigned short)((unsigned char)(G))<<8))|(((unsigned long)(unsigned char)(B))<<16)))

#define KgGetRed(RGB)			((RGB)& 0xff)
#define KgGetGreen(RGB)			((((unsigned short)(RGB)) >> 8)& 0xff)
#define KgGetBlue(RGB)			(((RGB)>>16)& 0xff)

struct CKgPoint {
	int X, Y;

	CKgPoint() {}
	CKgPoint(int x, int y) : X(x), Y(y) {}

	CKgPoint operator+ (CKgPoint p1);
	CKgPoint &operator+= (CKgPoint p1);
};

struct CKgLine {
	CKgPoint Start, End;

	CKgLine() {}
	CKgLine(CKgPoint s, CKgPoint e) : Start(s), End(e) {}
	CKgLine(int sx, int sy, int ex, int ey) : Start(CKgPoint(sx, sy)), End(CKgPoint(ex, ey)) {}
};

struct CKgRect {
	int Left, Top, Right, Bottom;

	CKgRect() : Left(0), Top(0), Right(0), Bottom(0) {}
	CKgRect(int l, int t, int r, int b) : Left(l), Top(t), Right(r), Bottom(b) {}

	bool IsRect();
	int Width();
	int Height();
	CKgPoint Center();
	void Move(int x, int y);
	void Intersect(CKgRect rt);
	void Union(CKgRect rt);
	void Expanded(int e);
};

class CKgVector2D 
{
public:
	double x, y;

	CKgVector2D() : x(0.), y(0.) {}
	CKgVector2D(double xx, double yy) : x(xx), y(yy) {}
	CKgVector2D(CKgPoint pt) : x(pt.X), y(pt.Y) {}

	static double abs(CKgVector2D v);
	void Normalize();
	double Dot(CKgVector2D v1);
	CKgVector2D operator+ (CKgVector2D v);
	CKgVector2D operator- (CKgVector2D v);
	CKgVector2D operator- ();
	CKgVector2D &operator+= (CKgVector2D v);
};
CKgVector2D operator*(double s, CKgVector2D v);

class CKgVector3D 
{
public:
	double x, y, z;

	CKgVector3D() : x(0.), y(0.), z(0.) {}
	CKgVector3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

	static double abs(CKgVector3D v);
	void Normalize();
	double Dot(CKgVector3D v1);
	CKgVector3D Cross(CKgVector3D v);
	CKgVector3D operator+ (CKgVector3D v);
	CKgVector3D operator- (CKgVector3D v);
	CKgVector3D operator- ();
	CKgVector3D &operator+= (CKgVector3D v);
};
CKgVector3D operator*(double s, CKgVector3D v);

unsigned char **cmatrix(int nH, int nW);
void free_cmatrix(unsigned char **Image, int nH, int nW);
double **dmatrix(int nH, int nW);
void free_dmatrix(double **Image, int nH, int nW);

void DrawLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color);

bool InverseMatrix(double  **a, double **y, int nN);

void FFT2Radix(double *Xr, double *Xi, double *Yr, double *Yi, int nN, bool bInverse);
void DCT2D(double **Input, double **Output, int nW, int nH, int nBlockSize);
void IDCT2D(double **Input, double **Output, int nW, int nH, int nBlockSize);

double GetMse(unsigned char **I, unsigned char **O, int nW, int nH);
double GetPsnr(unsigned char **IR, unsigned char **IG, unsigned char **IB, unsigned char **OR, unsigned char **OG, unsigned char **OB,
	int nW, int nH);

