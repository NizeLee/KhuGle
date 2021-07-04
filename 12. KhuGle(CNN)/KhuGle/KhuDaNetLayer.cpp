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
	else if(m_LayerOption.nActicationFn == KDN_AF_TANH)
	{
		Activation = CKhuDaNet::TanH;
		DifferentialActivation = CKhuDaNet::DifferentialTanH;
	}
	else if(m_LayerOption.nActicationFn == KDN_AF_RELU)
	{
		Activation = CKhuDaNet::Relu;
		DifferentialActivation = CKhuDaNet::DifferentialRelu;
	}
	else if(m_LayerOption.nActicationFn == KDN_AF_LEAKY_RELU)
	{
		Activation = CKhuDaNet::LeakyRelu;
		DifferentialActivation = CKhuDaNet::DifferentialLeakyRelu;
	}
	else if(m_LayerOption.nActicationFn == KDN_AF_SOFTMAX)
	{
		Activation = CKhuDaNet::Softmax;
		DifferentialActivation = CKhuDaNet::DifferentialSoftmax;
	}

	m_pBackwardLayer = pBackwardLayerInput;

	if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_CON||m_LayerOption.nLayerType & KDN_LT_POOL))
		m_LayerOption.nNodeCnt = m_LayerOption.nW*m_LayerOption.nH*m_LayerOption.nImageCnt;

	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		m_Loss = new double [m_LayerOption.nNodeCnt];
	}

	if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		m_Node = new double [m_LayerOption.nNodeCnt];
	}
	else if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_CON||m_LayerOption.nLayerType & KDN_LT_POOL))
	{
		m_NodeCnnImage = new double **[m_LayerOption.nImageCnt];
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			m_NodeCnnImage[i] = CKhuDaNet::dmatrix1d(m_LayerOption.nH, m_LayerOption.nW);
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		m_Node = new double [m_LayerOption.nNodeCnt];
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			m_Weight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
			m_Weight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH);
		m_Bias = new double[m_LayerOption.nNodeCnt];
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		m_NodeCnnImage = new double **[m_LayerOption.nImageCnt];
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			m_NodeCnnImage[i] = CKhuDaNet::dmatrix1d(m_LayerOption.nH, m_LayerOption.nW);

		m_CnnWeight = new double ***[m_LayerOption.nImageCnt];
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			m_CnnWeight[i] = new double **[m_pBackwardLayer->m_LayerOption.nImageCnt];
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				m_CnnWeight[i][j] = CKhuDaNet::dmatrix1d(m_LayerOption.nKernelSize, m_LayerOption.nKernelSize);
		}

		m_Bias = new double[m_LayerOption.nImageCnt];
	}
	else if(m_LayerOption.nLayerType & KDN_LT_POOL)
	{
		m_NodeCnnImage = new double **[m_LayerOption.nImageCnt];
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			m_NodeCnnImage[i] = CKhuDaNet::dmatrix1d(m_LayerOption.nH, m_LayerOption.nW);
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
	else if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_CON||m_LayerOption.nLayerType & KDN_LT_POOL))
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			CKhuDaNet::free_dmatrix1d(m_NodeCnnImage[i], m_LayerOption.nH, m_LayerOption.nW);
		delete [] m_NodeCnnImage;
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		delete [] m_Node;
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
			CKhuDaNet::free_dmatrix1d(m_Weight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
			CKhuDaNet::free_dmatrix1d(m_Weight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH);
		delete [] m_Bias;

		if(m_bTrained)
		{
			delete [] m_DeltaNode;
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
				CKhuDaNet::free_dmatrix1d(m_DeltaWeight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
			else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
				CKhuDaNet::free_dmatrix1d(m_DeltaWeight, m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH);
			delete [] m_DeltaBias;
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			CKhuDaNet::free_dmatrix1d(m_NodeCnnImage[i], m_LayerOption.nH, m_LayerOption.nW);
		delete [] m_NodeCnnImage;

		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				CKhuDaNet::free_dmatrix1d(m_CnnWeight[i][j], m_LayerOption.nKernelSize, m_LayerOption.nKernelSize);
			delete [] m_CnnWeight[i];
		}
		delete [] m_CnnWeight;

		delete [] m_Bias;

		if(m_bTrained)
		{
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
				CKhuDaNet::free_dmatrix1d(m_DeltaCnnImage[i], m_LayerOption.nH, m_LayerOption.nW);
			delete [] m_DeltaCnnImage;

			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
					CKhuDaNet::free_dmatrix1d(m_DeltaCnnWeight[i][j], m_LayerOption.nKernelSize, m_LayerOption.nKernelSize);
				delete [] m_DeltaCnnWeight[i];
			}
			delete [] m_DeltaCnnWeight;

			delete [] m_DeltaBias;
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_POOL)
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			CKhuDaNet::free_dmatrix1d(m_NodeCnnImage[i], m_LayerOption.nH, m_LayerOption.nW);
		delete [] m_NodeCnnImage;

		if(m_bTrained)
		{
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
				CKhuDaNet::free_dmatrix1d(m_DeltaCnnImage[i], m_LayerOption.nH, m_LayerOption.nW);
			delete [] m_DeltaCnnImage;
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
	else if((m_LayerOption.nLayerType & KDN_LT_INPUT) && (m_LayerOption.nLayerType & KDN_LT_CON||m_LayerOption.nLayerType & KDN_LT_POOL))
	{
	}
	else if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		if(!m_bTrained)
		{
			m_DeltaNode = new double [m_LayerOption.nNodeCnt];
			if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
				m_DeltaWeight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nNodeCnt);
			else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
				m_DeltaWeight = CKhuDaNet::dmatrix1d(m_LayerOption.nNodeCnt, m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH);
			m_DeltaBias = new double[m_LayerOption.nNodeCnt];
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		if(!m_bTrained)
		{
			m_DeltaCnnImage = new double **[m_LayerOption.nImageCnt];
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
				m_DeltaCnnImage[i] = CKhuDaNet::dmatrix1d(m_LayerOption.nH, m_LayerOption.nW);

			m_DeltaCnnWeight = new double ***[m_LayerOption.nImageCnt];
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			{
				m_DeltaCnnWeight[i] = new double **[m_pBackwardLayer->m_LayerOption.nImageCnt];
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
					m_DeltaCnnWeight[i][j] = CKhuDaNet::dmatrix1d(m_LayerOption.nKernelSize, m_LayerOption.nKernelSize);
			}

			m_DeltaBias = new double[m_LayerOption.nImageCnt];
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_POOL)
	{
		if(!m_bTrained)
		{
			m_DeltaCnnImage = new double **[m_LayerOption.nImageCnt];
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
				m_DeltaCnnImage[i] = CKhuDaNet::dmatrix1d(m_LayerOption.nH, m_LayerOption.nW);
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
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
			var = sqrt(2./(m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH + m_LayerOption.nNodeCnt));

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
			else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
			{
				int nSequenceIndex = 0;
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
					for(int y = 0 ; y < m_pBackwardLayer->m_LayerOption.nH ; ++y)
						for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW ; ++x)
						{
							m_Weight[i][nSequenceIndex] = distribution(generator);
							
							++nSequenceIndex;
						}
			}

			m_Bias[i] = 0;
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		double var = sqrt(2./(m_LayerOption.nKernelSize * m_LayerOption.nKernelSize * (m_pBackwardLayer->m_LayerOption.nImageCnt + m_LayerOption.nImageCnt)));
		std::normal_distribution<double> distribution(0., var);

		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				for(int y = 0 ; y < m_LayerOption.nKernelSize ; ++y)
					for(int x = 0 ; x < m_LayerOption.nKernelSize ; ++x)
					{
						m_CnnWeight[i][j][y][x] = distribution(generator);
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
			double Sum = 0;
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
				Sum += m_pBackwardLayer->m_Node[j] * m_Weight[i][j];

			m_Node[i] = Activation(Sum + m_Bias[i]);
		}
	}
	else if((m_LayerOption.nLayerType & KDN_LT_FC) && 
		((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL)))
	{
		for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
		{
			double Sum = 0;

			int nSequenceIndex = 0;
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				for(int y = 0 ; y < m_pBackwardLayer->m_LayerOption.nH ; ++y)
					for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW ; ++x)
						Sum += m_pBackwardLayer->m_NodeCnnImage[j][y][x] * m_Weight[i][nSequenceIndex++];

			m_Node[i] = Activation(Sum + m_Bias[i]);
		}
	}
	else if((m_LayerOption.nLayerType & KDN_LT_CON) && 
		((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL)))
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			for(int y = 0 ; y < m_LayerOption.nH ; ++y)
				for(int x = 0 ; x < m_LayerOption.nW ; ++x)
				{
					double Sum = 0;
					for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
						for(int dy = 0 ; dy < m_LayerOption.nKernelSize ; ++dy)
							for(int dx = 0 ; dx < m_LayerOption.nKernelSize ; ++dx)
								Sum += 
									m_pBackwardLayer->m_NodeCnnImage[j][y+dy][x+dx]*m_CnnWeight[i][j][dy][dx];

					m_NodeCnnImage[i][y][x] = Activation(Sum + m_Bias[i]);
				}
			
		}
	}
	else if((m_LayerOption.nLayerType & KDN_LT_POOL) && 
		((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL)))
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)	
		{
			for(int y = 0 ; y < m_LayerOption.nH ; ++y)
				for(int x = 0 ; x < m_LayerOption.nW ; ++x)
				{
					m_NodeCnnImage[i][y][x] = m_pBackwardLayer->m_NodeCnnImage[i][y*m_LayerOption.nKernelSize][x*m_LayerOption.nKernelSize];
					for(int py = y*m_LayerOption.nKernelSize ; py < (y+1)*m_LayerOption.nKernelSize ; ++py)
						for(int px = x*m_LayerOption.nKernelSize ; px < (x+1)*m_LayerOption.nKernelSize ; ++px)
						{
							if(m_NodeCnnImage[i][y][x] < m_pBackwardLayer->m_NodeCnnImage[i][py][px])
								m_NodeCnnImage[i][y][x] = m_pBackwardLayer->m_NodeCnnImage[i][py][px];
						}
				}
		}
	}

	int nMaxNode = 0;
	double Sum = 0;
	if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && (m_LayerOption.nLayerType & KDN_LT_FC))
	{
		for(int i = 1 ; i < m_LayerOption.nNodeCnt ; ++i)
			if(m_Node[i] > m_Node[nMaxNode])
				nMaxNode = i;

		for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			Sum += exp(m_Node[i]);
	}
	else if((m_LayerOption.nLayerType & KDN_LT_OUTPUT) && ((m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL)))
	{
	}

	if(Probability && Sum > 0) *Probability = exp(m_Node[nMaxNode])/Sum;

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
			if(m_LayerOption.nActicationFn == KDN_AF_SOFTMAX)
			{

				double Sum = 0;
				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					Sum += m_Loss[i] = exp(m_Node[i]);

				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_Loss[i] /= Sum;

				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_DeltaNode[i] = (Output[i] - m_Loss[i]) * DifferentialActivation(m_Node[i]);

				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_Loss[i] = -log(m_Loss[i])*Output[i];
			}
			else
			{
				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_DeltaNode[i] = (Output[i]-m_Node[i]) * DifferentialActivation(m_Node[i]);

				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_Loss[i] = (Output[i]-m_Node[i])*(Output[i]-m_Node[i]);
			}

		}
		else if((m_LayerOption.nLayerType & KDN_LT_CON) || (m_LayerOption.nLayerType & KDN_LT_POOL))
		{
		}
	}

	if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_INPUT)
		return;
	
	if(m_LayerOption.nLayerType & KDN_LT_FC)
	{
		if(m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
		{
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nNodeCnt ; ++j )
			{
				m_pBackwardLayer->m_DeltaNode[j] = 0;
				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
					m_pBackwardLayer->m_DeltaNode[j] += m_DeltaNode[i] * m_Weight[i][j];

				m_pBackwardLayer->m_DeltaNode[j] *= m_pBackwardLayer->DifferentialActivation(m_pBackwardLayer->m_Node[j]);
			}
		}
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
		{
			int nSequenceIndex = 0;
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				for(int y = 0 ; y < m_pBackwardLayer->m_LayerOption.nH ; ++y)
					for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW ; ++x)
					{
						double DeltaSum = 0;
						for(int i = 0 ; i < m_LayerOption.nNodeCnt ; i++)
							DeltaSum += m_DeltaNode[i] * m_Weight[i][nSequenceIndex];
						nSequenceIndex++;

						m_pBackwardLayer->m_DeltaCnnImage[j][y][x] = 
							DeltaSum*m_pBackwardLayer->DifferentialActivation(m_pBackwardLayer->m_NodeCnnImage[j][y][x]);
					}
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
		{
			memset(m_pBackwardLayer->m_DeltaCnnImage[j][0], 0, m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH*sizeof(double));
			
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
				for(int y = 0 ; y < m_LayerOption.nH ; ++y)
					for(int x = 0 ; x < m_LayerOption.nW ; ++x)
						for(int dy = 0 ; dy < m_LayerOption.nKernelSize ; ++dy)
							for(int dx = 0 ; dx < m_LayerOption.nKernelSize ; ++dx)
							{
								m_pBackwardLayer->m_DeltaCnnImage[j][y+dy][x+dx] += 
									m_CnnWeight[i][j][dy][dx] * m_DeltaCnnImage[i][y][x];
							}

			for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH ; ++x)
				m_pBackwardLayer->m_DeltaCnnImage[j][0][x] *= DifferentialActivation(m_pBackwardLayer->m_NodeCnnImage[j][0][x]);
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_POOL)
	{
		for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)	
		{
			for(int y = 0 ; y < m_LayerOption.nH ; ++y)
				for(int x = 0 ; x < m_LayerOption.nW ; ++x)
				{
					int x0 = x*m_LayerOption.nKernelSize;
					int y0 = y*m_LayerOption.nKernelSize;

					for(int py = y*m_LayerOption.nKernelSize ; py < (y+1)*m_LayerOption.nKernelSize ; ++py)
						for(int px = x*m_LayerOption.nKernelSize ; px < (x+1)*m_LayerOption.nKernelSize ; ++px)
						{
							m_pBackwardLayer->m_DeltaCnnImage[j][py][px] = 0;

							if(m_pBackwardLayer->m_NodeCnnImage[j][py][px] > m_pBackwardLayer->m_NodeCnnImage[j][y0][x0])
							{ 
								x0 = px;
								y0 = py;
							}
						}

					m_pBackwardLayer->m_DeltaCnnImage[j][y0][x0] = m_DeltaCnnImage[j][y][x];
				}
		}
	}
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
			else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
			{
				for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
				{
					int nSequenceIndex = 0;
					for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
						for(int y = 0 ; y < m_pBackwardLayer->m_LayerOption.nH ; ++y)
							for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW ; ++x)
								m_DeltaWeight[i][nSequenceIndex++] = 0;

					m_DeltaBias[i] = 0;
				}
			}
		}
		else if(m_LayerOption.nLayerType & KDN_LT_CON)
		{
			for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				{
					for(int dy = 0 ; dy < m_LayerOption.nKernelSize ; ++dy)
						for(int dx = 0 ; dx < m_LayerOption.nKernelSize ; ++dx)
							m_DeltaCnnWeight[i][j][dy][dx] = 0;
				}

				m_DeltaBias[i] = 0;
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
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
		{
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			{
				int nSequenceIndex = 0;
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
					for(int y = 0 ; y < m_pBackwardLayer->m_LayerOption.nH ; ++y)
						for(int x = 0 ; x < m_pBackwardLayer->m_LayerOption.nW ; ++x)
							m_DeltaWeight[i][nSequenceIndex++] += m_DeltaNode[i] * m_pBackwardLayer->m_NodeCnnImage[j][y][x];

				m_DeltaBias[i] += m_DeltaNode[i];
			}
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
				for(int dy = 0 ; dy < m_LayerOption.nKernelSize ; ++dy)
					for(int dx = 0 ; dx < m_LayerOption.nKernelSize ; ++dx)
						for(int y = 0 ; y < m_LayerOption.nH ; ++y)
							for(int x = 0 ; x < m_LayerOption.nW ; ++x)
							{
								m_DeltaCnnWeight[i][j][dy][dx] += m_pBackwardLayer->m_NodeCnnImage[j][dy+y][dx+x] * m_DeltaCnnImage[i][y][x];
							}
		}

		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
			for(int y = 0 ; y < m_LayerOption.nH ; ++y)
				for(int x = 0 ; x < m_LayerOption.nW ; ++x)
					m_DeltaBias[i] += m_DeltaCnnImage[i][y][x];
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
		else if((m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (m_pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
		{
			for(int i = 0 ; i < m_LayerOption.nNodeCnt ; ++i)
			{
				for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt*m_pBackwardLayer->m_LayerOption.nW*m_pBackwardLayer->m_LayerOption.nH ; ++j )
					m_Weight[i][j] += m_LayerOption.dLearningRate * m_DeltaWeight[i][j]/nBatchSize;

				m_Bias[i] += m_LayerOption.dLearningRate * m_DeltaBias[i]/nBatchSize;
			}
		}
	}
	else if(m_LayerOption.nLayerType & KDN_LT_CON)
	{
		for(int i = 0 ; i < m_LayerOption.nImageCnt ; ++i)
		{
			for(int j = 0 ; j < m_pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
			{
				for(int dy = 0 ; dy < m_LayerOption.nKernelSize ; ++dy)
					for(int dx = 0 ; dx < m_LayerOption.nKernelSize ; ++dx)
					{
						m_CnnWeight[i][j][dy][dx] += m_LayerOption.dLearningRate * m_DeltaCnnWeight[i][j][dy][dx]/nBatchSize;
					}
			}

			m_Bias[i] += m_LayerOption.dLearningRate * m_DeltaBias[i]/nBatchSize;
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