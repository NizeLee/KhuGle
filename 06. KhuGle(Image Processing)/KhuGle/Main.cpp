//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include "KhuGleSignal.h"
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

class CKhuGleImageLayer : public CKhuGleLayer {
public:
	CKhuGleSignal m_Image, m_ImageOut;

	CKhuGleImageLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0))
		: CKhuGleLayer(nW, nH, bgColor, ptPos) {}
	void DrawBackgroundImage();
};

void CKhuGleImageLayer::DrawBackgroundImage()
{
	for(int y = 0 ; y < m_nH ; y++)
		for(int x = 0 ; x < m_nW ; x++)
		{
			m_ImageBgR[y][x] = KgGetRed(m_bgColor);
			m_ImageBgG[y][x] = KgGetGreen(m_bgColor);
			m_ImageBgB[y][x] = KgGetBlue(m_bgColor);
		}

	if(m_Image.m_Red && m_Image.m_Green && m_Image.m_Blue)
	{
		for(int y = 0 ; y < m_Image.m_nH && y < m_nH ; ++y)
			for(int x = 0 ; x < m_Image.m_nW && x < m_nW ; ++x)
			{
				m_ImageBgR[y][x] = m_Image.m_Red[y][x];
				m_ImageBgG[y][x] = m_Image.m_Green[y][x];
				m_ImageBgB[y][x] = m_Image.m_Blue[y][x];
			}
	}

	if(m_ImageOut.m_Red && m_ImageOut.m_Green && m_ImageOut.m_Blue)
	{
		int OffsetX = 300, OffsetY = 0;
		for(int y = 0 ; y < m_ImageOut.m_nH && y + OffsetY < m_nH ; ++y)
			for(int x = 0 ; x < m_ImageOut.m_nW && x + OffsetX < m_nW ; ++x)
			{
				m_ImageBgR[y + OffsetY][x + OffsetX] = m_ImageOut.m_Red[y][x];
				m_ImageBgG[y + OffsetY][x + OffsetX] = m_ImageOut.m_Green[y][x];
				m_ImageBgB[y + OffsetY][x + OffsetX] = m_ImageOut.m_Blue[y][x];
			}
	}
}

class CImageProcessing : public CKhuGleWin {
public:
	CKhuGleImageLayer *m_pImageLayer;

	CImageProcessing(int nW, int nH, char *ImagePath);
	void Update();
};

CImageProcessing::CImageProcessing(int nW, int nH, char *ImagePath) 
	: CKhuGleWin(nW, nH) {
	m_pScene = new CKhuGleScene(640, 480, KG_COLOR_24_RGB(100, 100, 150));

	m_pImageLayer = new CKhuGleImageLayer(600, 420, KG_COLOR_24_RGB(150, 150, 200), CKgPoint(20, 30));
	m_pImageLayer->m_Image.ReadBmp(ImagePath);
	m_pImageLayer->m_ImageOut.ReadBmp(ImagePath);
	m_pImageLayer->DrawBackgroundImage();
	m_pScene->AddChild(m_pImageLayer);
}

void CImageProcessing::Update()
{
	if(m_bKeyPressed['D'] || m_bKeyPressed['I'] || m_bKeyPressed['C']
		|| m_bKeyPressed['E'] || m_bKeyPressed['M'])
	{
		bool bInverse = m_bKeyPressed['I'];
		bool bCompression = m_bKeyPressed['C'];
		bool bEdge = m_bKeyPressed['E'];
		bool bMean = m_bKeyPressed['M'];

		double **InputR = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		double **InputG = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		double **InputB = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);

		double **OutR = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		double **OutG = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		double **OutB = dmatrix(m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);

		for(int y = 0 ; y < m_pImageLayer->m_Image.m_nH ; ++y)
			for(int x = 0 ; x < m_pImageLayer->m_Image.m_nW ; ++x)
			{
				InputR[y][x] = m_pImageLayer->m_Image.m_Red[y][x];
				InputG[y][x] = m_pImageLayer->m_Image.m_Green[y][x];
				InputB[y][x] = m_pImageLayer->m_Image.m_Blue[y][x];
			}

		if(bEdge)
		{
			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					OutR[y][x] = OutG[y][x] = OutB[y][x] = 0.;
					if(x > 0 && x < m_pImageLayer->m_ImageOut.m_nW-1 && 
						y > 0 && y < m_pImageLayer->m_ImageOut.m_nH-1)
					{
						double Rx = InputR[y-1][x-1] + 2*InputR[y][x-1] + InputR[y+1][x-1]
							- InputR[y-1][x+1] - 2*InputR[y][x+1] - InputR[y+1][x+1];
						double Ry = InputR[y-1][x-1] + 2*InputR[y-1][x] + InputR[y-1][x+1]
							- InputR[y+1][x-1] - 2*InputR[y+1][x] - InputR[y+1][x+1];
						double Gx = InputG[y-1][x-1] + 2*InputG[y][x-1] + InputG[y+1][x-1]
							- InputG[y-1][x+1] - 2*InputG[y][x+1] - InputG[y+1][x+1];
						double Gy = InputG[y-1][x-1] + 2*InputG[y-1][x] + InputG[y-1][x+1]
							- InputG[y+1][x-1] - 2*InputG[y+1][x] - InputG[y+1][x+1];
						double Bx = InputB[y-1][x-1] + 2*InputB[y][x-1] + InputB[y+1][x-1]
							- InputB[y-1][x+1] - 2*InputB[y][x+1] - InputB[y+1][x+1];
						double By = InputB[y-1][x-1] + 2*InputB[y-1][x] + InputB[y-1][x+1]
							- InputB[y+1][x-1] - 2*InputB[y+1][x] - InputB[y+1][x+1];
						
						OutR[y][x] = sqrt(Rx*Rx + Ry*Ry);
						OutG[y][x] = sqrt(Gx*Gx + Gy*Gy);
						OutB[y][x] = sqrt(Bx*Bx + By*By);
					}
				}

			std::cout << "Edge" << std::endl;
		}
		else if(bMean)
		{
			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					OutR[y][x] = OutG[y][x] = OutB[y][x] = 0.;
					if(x > 0 && x < m_pImageLayer->m_ImageOut.m_nW-1 && 
						y > 0 && y < m_pImageLayer->m_ImageOut.m_nH-1)
					{
						for(int dy = -1 ; dy < 2 ; ++dy)
							for(int dx = -1 ; dx < 2 ; ++dx)
							{
								OutR[y][x] += InputR[y+dy][x+dx];
								OutG[y][x] += InputG[y+dy][x+dx];
								OutB[y][x] += InputB[y+dy][x+dx];
							}
					}
				}

			std::cout << "Mean filter" << std::endl;
		}
		else
		{
			DCT2D(InputR, OutR, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);
			DCT2D(InputG, OutG, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);
			DCT2D(InputB, OutB, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);

			std::cout << "DCT" << std::endl;
		}

		if(!bInverse && ! bCompression)
		{
			double MaxR, MaxG, MaxB, MinR, MinG, MinB;

			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					if(x == 0 && y == 0)
					{
						MaxR = MinR = OutR[y][x];
						MaxG = MinG = OutG[y][x];
						MaxB = MinB = OutB[y][x];
					}
					else
					{
						if(OutR[y][x] > MaxR) MaxR = OutR[y][x];
						if(OutG[y][x] > MaxG) MaxG = OutG[y][x];
						if(OutB[y][x] > MaxB) MaxB = OutB[y][x];

						if(OutR[y][x] < MinR) MinR = OutR[y][x];
						if(OutG[y][x] < MinG) MinG = OutG[y][x];
						if(OutB[y][x] < MinB) MinB = OutB[y][x];
					}
				}
			
			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					if(MaxR == MinR) m_pImageLayer->m_ImageOut.m_Red[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Red[y][x] = (int)((OutR[y][x]-MinR)*255/(MaxR-MinR));
					if(MaxG == MinG) m_pImageLayer->m_ImageOut.m_Green[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Green[y][x] = (int)((OutG[y][x]-MinG)*255/(MaxG-MinG));
					if(MaxB == MinB) m_pImageLayer->m_ImageOut.m_Blue[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Blue[y][x] = (int)((OutB[y][x]-MinB)*255/(MaxB-MinB));
				}
		}
		else
		{
			if(bCompression)
			{
				for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
					for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
					{
						if(x%8 > 3 || y %8 > 3)
						{
							OutR[y][x] = 0;
							OutG[y][x] = 0;
							OutB[y][x] = 0;
						}
					}

				std::cout << "Compression" << std::endl;
			}
			else
				std::cout << "Non compression" << std::endl;

			IDCT2D(OutR, InputR, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);
			IDCT2D(OutG, InputG, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);
			IDCT2D(OutB, InputB, m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH, 8);

			double MaxR, MaxG, MaxB, MinR, MinG, MinB;

			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					if(x == 0 && y == 0)
					{
						MaxR = MinR = InputR[y][x];
						MaxG = MinG = InputG[y][x];
						MaxB = MinB = InputB[y][x];
					}
					else
					{
						if(InputR[y][x] > MaxR) MaxR = InputR[y][x];
						if(InputG[y][x] > MaxG) MaxG = InputG[y][x];
						if(InputB[y][x] > MaxB) MaxB = InputB[y][x];

						if(InputR[y][x] < MinR) MinR = InputR[y][x];
						if(InputG[y][x] < MinG) MinG = InputG[y][x];
						if(InputB[y][x] < MinB) MinB = InputB[y][x];
					}
				}

			for(int y = 0 ; y < m_pImageLayer->m_ImageOut.m_nH ; ++y)
				for(int x = 0 ; x < m_pImageLayer->m_ImageOut.m_nW ; ++x)
				{
					if(MaxR == MinR) m_pImageLayer->m_ImageOut.m_Red[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Red[y][x] = (int)((InputR[y][x]-MinR)*255/(MaxR-MinR));
					if(MaxG == MinG) m_pImageLayer->m_ImageOut.m_Green[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Green[y][x] = (int)((InputG[y][x]-MinG)*255/(MaxG-MinG));
					if(MaxB == MinB) m_pImageLayer->m_ImageOut.m_Blue[y][x] = 0;
					else m_pImageLayer->m_ImageOut.m_Blue[y][x] = (int)((InputB[y][x]-MinB)*255/(MaxB-MinB));
				}
		}

		if(bMean || bCompression || bInverse)
		{
			double Psnr = GetPsnr(m_pImageLayer->m_Image.m_Red, m_pImageLayer->m_Image.m_Green, m_pImageLayer->m_Image.m_Blue, 
				m_pImageLayer->m_ImageOut.m_Red, m_pImageLayer->m_ImageOut.m_Green, m_pImageLayer->m_ImageOut.m_Blue, 
				m_pImageLayer->m_Image.m_nW, m_pImageLayer->m_Image.m_nH);

			std::cout << Psnr << std::endl;
		}

		free_dmatrix(InputR, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		free_dmatrix(InputG, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		free_dmatrix(InputB, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);

		free_dmatrix(OutR, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		free_dmatrix(OutG, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);
		free_dmatrix(OutB, m_pImageLayer->m_Image.m_nH, m_pImageLayer->m_Image.m_nW);

		m_pImageLayer->DrawBackgroundImage();

		m_bKeyPressed['D'] = m_bKeyPressed['I'] = m_bKeyPressed['C']
			= m_bKeyPressed['E'] = m_bKeyPressed['M'] = false;
	}

	m_pScene->Render();
	DrawSceneTextPos("Image Processing", CKgPoint(0, 0));
	
	CKhuGleWin::Update();
}

int main()
{
	char ExePath[MAX_PATH], ImagePath[MAX_PATH];

	GetModuleFileName(NULL, ExePath, MAX_PATH);

	int i;
	int LastBackSlash = -1;
	int nLen = strlen(ExePath);
	for(i = nLen-1 ; i >= 0 ; i--)
	{
		if(ExePath[i] == '\\') {
			LastBackSlash = i;
			break;
		}
	}

	if(LastBackSlash >= 0)
		ExePath[LastBackSlash] = '\0';

	sprintf(ImagePath, "%s\\%s", ExePath, "couple.bmp");

	CImageProcessing *pImageProcessing = new CImageProcessing(640, 480, ImagePath);
	KhuGleWinInit(pImageProcessing);

	return 0;
}