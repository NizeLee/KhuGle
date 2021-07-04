//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include "KhuGleBase.h"
#include "KhuGleComponent.h"

class CKhuGleLayer : public CKhuGleComponent
{
public:
	bool m_bInit;
	int m_nW, m_nH;
	CKgPoint m_ptPos;

	unsigned char **m_ImageR, **m_ImageG, **m_ImageB;
	unsigned char **m_ImageBgR, **m_ImageBgG, **m_ImageBgB;
	KgColor24 m_bgColor;

	CKhuGleLayer(int nW, int nH, KgColor24 bgColor, CKgPoint ptPos = CKgPoint(0, 0));
	~CKhuGleLayer();

	void SetBackgroundImage(int nW, int nH, KgColor24 bgColor);
	void ResetBackgroundImage();
	void SetBgColor(KgColor24 bgColor);

	virtual void Render();
};

