//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <iostream>

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

struct CKgTriangle{
	CKgVector3D v0, v1, v2;

	CKgTriangle() : v0(CKgVector3D()), v1(CKgVector3D()), v2(CKgVector3D()) {};
	CKgTriangle(CKgVector3D vv0, CKgVector3D vv1, CKgVector3D vv2)
		: v0(vv0), v1(vv1), v2(vv2) {};
}; 

class CKhuGle3DSprite : public CKhuGleSprite {
public:
	std::vector<CKgTriangle> SurfaceMesh;
	double **m_ProjectionMatrix;
	CKgVector3D m_CameraPos;

	CKhuGle3DSprite(int nW, int nH, double Fov, double Far, double Near, KgColor24 fgColor);
	~CKhuGle3DSprite();

	static void DrawTriangle(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, 
		int x0, int y0, int x1, int y1, int x2, int y2, KgColor24 Color24);
	static void MatrixVector44(CKgVector3D &out, CKgVector3D v, double **M);
	static double **ComputeViewMatrix(CKgVector3D Camera, CKgVector3D Target, CKgVector3D CameraUp);

	void Render();
	void MoveBy(double OffsetX, double OffsetY, double OffsetZ);
};

CKhuGle3DSprite::CKhuGle3DSprite(int nW, int nH, double Fov, double Far, double Near, KgColor24 fgColor) {
	m_fgColor = fgColor;
	m_CameraPos = CKgVector3D(0., -0.2, -2);

	m_ProjectionMatrix = dmatrix(4, 4);
	for(int r = 0 ; r < 4 ; ++r)
		for(int c = 0 ; c < 4 ; ++c)
			m_ProjectionMatrix[r][c] = 0.;

	m_ProjectionMatrix[0][0] = (double)nH/(double)nW * 1./tan(Fov/2.);
	m_ProjectionMatrix[1][1] = 1./tan(Fov/2.);
	m_ProjectionMatrix[2][2] = (-Near-Far) / (Near-Far);
	m_ProjectionMatrix[2][3] = 2.*(Far * Near) / (Near-Far);
	m_ProjectionMatrix[3][2] = 1.;
	m_ProjectionMatrix[3][3] = 0.;	
	
	SurfaceMesh.push_back(CKgTriangle(CKgVector3D(-0.5, 0., -sqrt(3.)/6), CKgVector3D(0.5, 0., -sqrt(3.)/6), CKgVector3D(0., 0., sqrt(3.)/3)));
	SurfaceMesh.push_back(CKgTriangle(CKgVector3D(0., 0., sqrt(3.)/3), CKgVector3D(0.5, 0., -sqrt(3.)/6), CKgVector3D(0., sqrt(3.)/3, 0.)));
	SurfaceMesh.push_back(CKgTriangle(CKgVector3D(-0.5, 0., -sqrt(3.)/6), CKgVector3D(0, 0., sqrt(3.)/3), CKgVector3D(0., sqrt(3.)/3, 0.)));
	SurfaceMesh.push_back(CKgTriangle(CKgVector3D(0.5, 0., -sqrt(3.)/6), CKgVector3D(-0.5, 0., -sqrt(3.)/6), CKgVector3D(0., sqrt(3.)/3, 0.)));																						   
};

CKhuGle3DSprite::~CKhuGle3DSprite() {
	free_dmatrix(m_ProjectionMatrix, 4, 4);
};

void CKhuGle3DSprite::DrawTriangle(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, 
	int x0, int y0, int x1, int y1, int x2, int y2, KgColor24 Color24)
{
	CKhuGleSprite::DrawLine(R, G, B, nW, nH, x0, y0, x1, y1, Color24);
	CKhuGleSprite::DrawLine(R, G, B, nW, nH, x1, y1, x2, y2, Color24);
	CKhuGleSprite::DrawLine(R, G, B, nW, nH, x2, y2, x0, y0, Color24);	
}

void CKhuGle3DSprite::MatrixVector44(CKgVector3D &out, CKgVector3D v, double **M)
{
	out.x = v.x*M[0][0] + v.y*M[0][1] + v.z*M[0][2] + M[0][3];
	out.y = v.x*M[1][0] + v.y*M[1][1] + v.z*M[1][2] + M[1][3];
	out.z = v.x*M[2][0] + v.y*M[2][1] + v.z*M[2][2] + M[2][3];

	double w = v.x*M[3][0] + v.y*M[3][1] + v.z*M[3][2] + M[3][3];

	if(fabs(w) > 0)
		out = (1./w)*out;
}

double **CKhuGle3DSprite::ComputeViewMatrix(CKgVector3D Camera, CKgVector3D Target, CKgVector3D CameraUp)
{
	CKgVector3D Forward = Target-Camera;
	Forward.Normalize();
	CameraUp.Normalize();
	CKgVector3D Right = CameraUp.Cross(Forward); 
	CKgVector3D Up = Forward.Cross(Right);

	double **RT = dmatrix(4, 4);
	double **View = dmatrix(4, 4);	

	RT[0][0] = Right.x;	
	RT[1][0] = Right.y;	
	RT[2][0] = Right.z;	
	RT[3][0] = 0.;
	RT[0][1] = Up.x;		
	RT[1][1] = Up.y;		
	RT[2][1] = Up.z;		
	RT[3][1] = 0.;
	RT[0][2] = Forward.x;	
	RT[1][2] = Forward.y;	
	RT[2][2] = Forward.z;	
	RT[3][2] = 0.;
	RT[0][3] = Camera.x;			
	RT[1][3] = Camera.y;			
	RT[2][3] = Camera.z;			
	RT[3][3] = 1.;

	bool bInverse = InverseMatrix(RT, View, 4);
	
	free_dmatrix(RT, 4, 4);

	if(bInverse)
		return View;

	return nullptr;

}

void CKhuGle3DSprite::Render()
{
	if(!m_Parent) return;

	double NewX = m_CameraPos.x*cos(Pi/1000.) - m_CameraPos.z*sin(Pi/1000.);
	double NewZ = m_CameraPos.x*sin(Pi/1000.) + m_CameraPos.z*cos(Pi/1000.);
	m_CameraPos.x = NewX;
	m_CameraPos.z = NewZ;

	CKhuGleLayer *Parent = (CKhuGleLayer *)m_Parent;

	double **ViewMatrix = ComputeViewMatrix(m_CameraPos, CKgVector3D(0., 0., 0.), CKgVector3D(0., 1., 0.));

	if(ViewMatrix == nullptr) return;

	for(auto &Triangle: SurfaceMesh)
	{
		CKgVector3D Side01, Side02, Normal;

		Side01 = Triangle.v1 - Triangle.v0;
		Side02 = Triangle.v2 - Triangle.v0;

		Normal = Side01.Cross(Side02);
		Normal.Normalize();

		CKgTriangle ViewTriangle;
		CKgTriangle Projected;
			
		if(Normal.Dot(Triangle.v0 - m_CameraPos) < 0.)
		{
			MatrixVector44(ViewTriangle.v0, Triangle.v0, ViewMatrix);
			MatrixVector44(ViewTriangle.v1, Triangle.v1, ViewMatrix);
			MatrixVector44(ViewTriangle.v2, Triangle.v2, ViewMatrix);
					
			MatrixVector44(Projected.v0, ViewTriangle.v0, m_ProjectionMatrix);
			MatrixVector44(Projected.v1, ViewTriangle.v1, m_ProjectionMatrix);
			MatrixVector44(Projected.v2, ViewTriangle.v2, m_ProjectionMatrix);

			Projected.v0.x += 1.; 
			Projected.v0.y += 1.;
			Projected.v1.x += 1.; 
			Projected.v1.y += 1.;
			Projected.v2.x += 1.; 
			Projected.v2.y += 1.;
			Projected.v0.x *= Parent->m_nW/2.;
			Projected.v0.y *= Parent->m_nH/2.;
			Projected.v1.x *= Parent->m_nW/2.;
			Projected.v1.y *= Parent->m_nH/2.;
			Projected.v2.x *= Parent->m_nW/2.;
			Projected.v2.y *= Parent->m_nH/2.;
			Projected.v0.x -= 1.; 
			Projected.v0.y -= 1.;
			Projected.v1.x -= 1.; 
			Projected.v1.y -= 1.;
			Projected.v2.x -= 1.; 
			Projected.v2.y -= 1.;

			DrawTriangle(Parent->m_ImageR, Parent->m_ImageG, Parent->m_ImageB, 
				Parent->m_nW, Parent->m_nH, 
				(int)Projected.v0.x, (int)Projected.v0.y, 
				(int)Projected.v1.x, (int)Projected.v1.y, 
				(int)Projected.v2.x, (int)Projected.v2.y, m_fgColor);
		}
	}

	free_dmatrix(ViewMatrix, 4, 4);
}

void CKhuGle3DSprite::MoveBy(double OffsetX, double OffsetY, double OffsetZ)
{
	for(auto &Triangle: SurfaceMesh)
	{
		Triangle.v0 = Triangle.v0 + CKgVector3D(OffsetX, OffsetY, OffsetZ);
		Triangle.v1 = Triangle.v1 + CKgVector3D(OffsetX, OffsetY, OffsetZ);
		Triangle.v2 = Triangle.v2 + CKgVector3D(OffsetX, OffsetY, OffsetZ);
	}
}

class CThreeDim : public CKhuGleWin
{
public:
	CKhuGleLayer *m_pGameLayer;

	CKhuGle3DSprite *m_pObject3D;
	
	CThreeDim(int nW, int nH);
	void Update();

	CKgPoint m_LButtonStart, m_LButtonEnd;
	int m_nLButtonStatus;
};

CThreeDim::CThreeDim(int nW, int nH) : CKhuGleWin(nW, nH) 
{
	m_nLButtonStatus = 0;

	m_Gravity = CKgVector2D(0., 98.);
	m_AirResistance = CKgVector2D(0.1, 0.1);

	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pGameLayer = new CKhuGleLayer(600, 420, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 30));
	m_pScene->AddChild(m_pGameLayer);

	m_pObject3D = new CKhuGle3DSprite(m_pGameLayer->m_nW, m_pGameLayer->m_nH, Pi/2., 1000., 0.1, KG_COLOR_24_RGB(255, 0, 255));

	m_pGameLayer->AddChild(m_pObject3D);
}

void CThreeDim::Update()
{
	if(m_bKeyPressed[VK_DOWN]) 
		m_pObject3D->MoveBy(0, 0.0005, 0);

	m_pScene->Render();
	DrawSceneTextPos("3D Rendering", CKgPoint(0, 0));

	CKhuGleWin::Update();
}

int main()
{
	CThreeDim *pThreeDim = new CThreeDim(640, 480);

	KhuGleWinInit(pThreeDim);

	return 0;
}