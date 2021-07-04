//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuGleComponent.h"

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

CKhuGleComponent::CKhuGleComponent()
{
	m_Parent = nullptr;
}

CKhuGleComponent::~CKhuGleComponent()
{
	for(auto &Child : m_Children)
		delete Child;
}

void CKhuGleComponent::AddChild(CKhuGleComponent *pChild)
{
	pChild->m_Parent = this;

	m_Children.push_back(pChild);
}
