//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
#include "KhuGleWin.h"
#include <iostream>

int main()
{
	CKhuGleWin *pKhuGleSample = new CKhuGleWin(640, 480);

	KhuGleWinInit(pKhuGleSample);

	return 0;
}