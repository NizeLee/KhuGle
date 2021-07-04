//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuDaNet.h"
#include "KhuDaNetLayer.h"

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <random>
#include <chrono>

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


CKhuDaNetLayerOption::CKhuDaNetLayerOption(unsigned int nLayerTypeIntput, int nImageCntInput, int nNodeCntIput, 
	int nWidthInput, int nHeightInput, int nKernelSizeInput, 
	int nActicationFnInput, double dLearningRateInput) 
{
	nLayerType = nLayerTypeIntput;
	nImageCnt = nImageCntInput;
	nNodeCnt = nNodeCntIput;
	nW = nWidthInput; 
	nH = nHeightInput;
	nKernelSize = nKernelSizeInput;
	nActicationFn = nActicationFnInput;

	dLearningRate = dLearningRateInput;
}

CKhuDaNetLayer::CKhuDaNetLayer(CKhuDaNetLayerOption m_LayerOptionInput, CKhuDaNetLayer *pBackwardLayerInput) : m_LayerOption(m_LayerOptionInput), m_bTrained(false)
{
	if(m_LayerOption.nActicationFn == KDN_AF_IDENTIFY)
	{
		Activation = CKhuDaNet::Identify;
		DifferentialActivation = CKhuDaNet::DifferentialIdentify;
	}
	else if(m_LayerOption.nActicationFn == KDN_AF_BINARY_STEP)
	{
		Activation = CKhuDaNet::BinaryStep;
		DifferentialActivation = CKhuDaNet::DifferentialBinaryStep;
	}
	else if(m_LayerOption.nActicationFn == KDN_AF_SIGMOID)
	{
		Activation = CKhuDaNet::Sigmoid;
		DifferentialActivation = CKhuDaNet::DifferentialSigmoid;
	}

	m_pBackwardLayer = pBackwardLayerInput;

	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		m_Loss = new double [m_LayerOption.nNodeCnt];
	}

	if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		m_Node = new double [m_LayerOption.nNodeCnt];
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		m_Node = new double [m_LayerOption.nNodeCnt];
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			m_Weight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
		
		m_Bias = new double[m_LayerOption.nNodeCnt];
	}
}

CKhuDaNetLayer::~CKhuDaNetLayer()
{
	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		delete [] m_Loss;
	}

	if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		delete [] m_Node;
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		delete [] m_Node;
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			CKhuDaNet::free_dmatrix1d(m_Weight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
		
		delete [] m_Bias;

		if(m_bTrained)
		{
			delete [] m_DeltaNode;
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
				CKhuDaNet::free_dmatrix1d(m_DeltaWeight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
		
			delete [] m_DeltaBias;
		}
	}
}

void CKhuDaNetLayer::AllocDeltaWeight()
{
	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
	}

	if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		if(!m_bTrained)
		{
			m_DeltaNode = new double [m_LayerOption.nNodeCnt];
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
				m_DeltaWeight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
	
			m_DeltaBias = new double[m_LayerOption.nNodeCnt];
		}
	}

	m_bTrained = true;
}

void CKhuDaNetLayer::InitWeight()
{
	static unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	static std::default_random_engine generator(seed);

	if(m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;

	if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		double var = 1;
		
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			var = sqrt(2./(m_pBackwardLayer->m_LayerOption.nNodeCnt + m_LayerOption.nNodeCnt));

		std::normal_distribution<double> distribution(0., var);

		for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
		{
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j)
				{
					m_Weight[i][j] = distribution(generator);
				}
			}

			m_Bias[i] = 0;
		}
	}
}

int CKhuDaNetLayer::ComputeLayer(double *Probability)
{
	if(m_LayerOption.nLayerType & KDN_LT_INPUT)
		return 0;

	if((m_LayerOption.nLayerType & KDN_LT_FC) && (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC))
	{
		for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
		{
			m_Node[i] = 0;
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
				m_Node[i] += m_pBackwardLayer->m_Node[j] * m_Weight[i][j];

			m_Node[i] = Activation(m_Node[i] + m_Bias[i]);
		}
	}

	int nMaxNode;

	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		if(m_Node[0] < 0.5) nMaxNode = 0;
		else nMaxNode = 1;
	}

	if(Probability) *Probability = 0;

	return nMaxNode;
}

void CKhuDaNetLayer::ComputeDelta(double *Output)
{
	if(m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;
	

	if(m_LayerOption.nLayerType & KDN_LT_OUTPUT)
	{
		if(m_LayerOption.nLayerType & KDN_LT_FC)
		{
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
				m_DeltaNode[i] = (Output[i]-m_Node[i]) * DifferentialActivation(m_Node[i]);
			
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
				m_Loss[i] = (Output[i]-m_Node[i])*(Output[i]-m_Node[i]);
		}
	}

	if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;
}

void CKhuDaNetLayer::ComputeDeltaWeight(bool bReset)
{
	if(m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;

	if(bReset)
	{
		if(m_LayerOption.nLayerType & KDN_LT_FC)
		{
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			{
				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
				{
					for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
						m_DeltaWeight[i][j] = 0;
		
					m_DeltaBias[i] = 0;
				}
			}
		}
	}

	if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
		{
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
					m_DeltaWeight[i][j] += m_DeltaNode[i] * m_pBackwardLayer->m_Node[j];

				m_DeltaBias[i] += m_DeltaNode[i];
			}
		}
	}
}

void CKhuDaNetLayer::UpdateWeight(int nBatchSize)
{
	if(m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;

	if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
		{
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
					m_Weight[i][j] += m_LayerOption.dLearningRate * m_DeltaWeight[i][j]/nBatchSize;

				m_Bias[i] += m_LayerOption.dLearningRate * m_DeltaBias[i]/nBatchSize;
			}
		}
	}
}

double CKhuDaNetLayer::GetLoss()
{
	double Loss = 0;
	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			Loss += m_Loss[i];
	}

	return Loss;
}