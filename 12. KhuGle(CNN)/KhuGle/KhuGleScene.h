//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#pragma once

#include "KhuGleBase.h"
#include "KhuGleComponent.h"

class CKhuGleScene : public CKhuGleComponent
{
public:
	bool m_bInit;
	int m_nW, m_nH;

	unsigned char **m_ImageR, **m_ImageG, **m_ImageB;
	KgColor24 m_bgColor;

	CKhuGleScene(int nW, int nH, KgColor24 bgColor);
	~CKhuGleScene();

	void SetBackgroundImage(int nW, int nH, KgColor24 bgColor);
	void ResetBackgroundImage();
	void SetBgColor(KgColor24 bgColor);

	virtual void Render();
};

