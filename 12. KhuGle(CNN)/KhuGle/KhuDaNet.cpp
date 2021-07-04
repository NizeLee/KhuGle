//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuDaNet.h"
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

CKhuDaNet::CKhuDaNet()
{
	m_nInputSize = m_nOutputSize = 0;

	m_Information = new char[MAX_INFORMATION_STRING_SIZE];
}


CKhuDaNet::~CKhuDaNet()
{
	ClearAllLayers();

	delete [] m_Information;
}

bool CKhuDaNet::IsNetwork()
{
	if(m_Layers.size() < 2) return false;
	return true;
}

char *CKhuDaNet::GetInformation()
{
	memset(m_Information, 32, MAX_INFORMATION_STRING_SIZE);

	m_Information[MAX_INFORMATION_STRING_SIZE-1] = 0;

	int nPos = 0;
	for(auto &Layer : m_Layers)
	{
		if(Layer->m_LayerOption.nLayerType & KDN_LT_FC)
		{
			sprintf(m_Information+nPos, "%s(%5d)  ", "FC", Layer->m_LayerOption.nNodeCnt);
			nPos += 10;
		}
		else if((Layer->m_LayerOption.nLayerType & KDN_LT_IMG) == KDN_LT_IMG)
		{
			sprintf(m_Information+nPos, "%s(%5d)  ", "IMG", Layer->m_LayerOption.nImageCnt);
			nPos += 11;
		}
		else if(Layer->m_LayerOption.nLayerType & KDN_LT_CON)
		{
			sprintf(m_Information+nPos, "%s(%5d)  ", "CON", Layer->m_LayerOption.nImageCnt);
			nPos += 11;
		}
		else if(Layer->m_LayerOption.nLayerType & KDN_LT_POOL)
		{
			sprintf(m_Information+nPos, "%s(%5d)  ", "POOL", Layer->m_LayerOption.nImageCnt);
			nPos += 12;
		}
	}

	return m_Information;
}

void CKhuDaNet::ClearAllLayers()
{
	for(std::vector<CKhuDaNetLayer*>::reverse_iterator Iter = m_Layers.rbegin(); Iter != m_Layers.rend(); ++Iter)
	{
		delete [] *Iter;
		*Iter = 0;
	}

	m_Layers.clear();
}

void CKhuDaNet::AddLayer(CKhuDaNetLayer *pLayer)
{
	if(m_Layers.size() == 0)
		m_nInputSize = pLayer->m_LayerOption.nNodeCnt;

	m_nOutputSize = pLayer->m_LayerOption.nNodeCnt;

	m_Layers.push_back(pLayer);
}

void CKhuDaNet::AddLayer(CKhuDaNetLayerOption LayerOptionInput)
{
	CKhuDaNetLayer *pLayer;

	if(m_Layers.size() == 0)
	{
		pLayer = new CKhuDaNetLayer(LayerOptionInput, nullptr);
		m_nInputSize = pLayer->m_LayerOption.nNodeCnt;
	}
	else 
		pLayer = new CKhuDaNetLayer(LayerOptionInput, m_Layers[m_Layers.size()-1]);

	m_nOutputSize = pLayer->m_LayerOption.nNodeCnt;

	m_Layers.push_back(pLayer);
}

void CKhuDaNet::InitWeight()
{
	for(auto &Layer : m_Layers)
		Layer->InitWeight();
}

void CKhuDaNet::AllocDeltaWeight()
{
	for(auto &Layer : m_Layers)
		Layer->AllocDeltaWeight();
}

int CKhuDaNet::Forward(double *Input, double *Probability)
{
	int MaxPos;

	for(auto &Layer : m_Layers)
	{
		if(Layer->m_LayerOption.nLayerType & KDN_LT_INPUT)
		{
			if(Layer->m_LayerOption.nLayerType & KDN_LT_FC)
				memcpy(Layer->m_Node, Input, Layer->m_LayerOption.nNodeCnt*sizeof(double));
			else if((Layer->m_LayerOption.nLayerType & KDN_LT_CON) || (Layer->m_LayerOption.nLayerType & KDN_LT_POOL))
			{
				int nPadding = m_Layers[1]->m_LayerOption.nKernelSize/2;
				int nSequenceIndex = 0;
				for(int i = 0 ; i < Layer->m_LayerOption.nImageCnt ; ++i)
					for(int y = 0 ; y < Layer->m_LayerOption.nH ; ++y)
						for(int x = 0 ; x < Layer->m_LayerOption.nW ; ++x)
						{
							if(x < nPadding || x >= Layer->m_LayerOption.nW - nPadding || 
								y < nPadding || y >= Layer->m_LayerOption.nH - nPadding) 
								Layer->m_NodeCnnImage[i][y][x] = 0;
							else
								Layer->m_NodeCnnImage[i][y][x] = Input[nSequenceIndex++];
						}
			}
		}
		else if(Layer->m_LayerOption.nLayerType & KDN_LT_OUTPUT)
			MaxPos = Layer->ComputeLayer(Probability);
		else 
			Layer->ComputeLayer();
	}

	return MaxPos;
}

int CKhuDaNet::TrainBatch(double **Input, double **Output, int nBatchSize, double *pLoss)
{
	int nTP = 0;
	*pLoss = 0;

	AllocDeltaWeight();

	for(int i = 0 ; i < nBatchSize ; ++i)
	{
		int MaxPos = Forward(Input[i]);
		
		if(m_nOutputSize == 1)
		{
			if(MaxPos == 1 && Output[i][0] > 0.5)
				nTP++;
			else if(MaxPos == 0 && Output[i][0] < 0.5)
				nTP++;

		}
		else
		{
			if(MaxPos == ArgMax(Output[i], m_nOutputSize))
				nTP++;
		}

		for(std::vector<CKhuDaNetLayer*>::reverse_iterator Iter = m_Layers.rbegin(); Iter != m_Layers.rend(); ++Iter)
		{
			(*Iter)->ComputeDelta(Output[i]);
			(*Iter)->ComputeDeltaWeight(i==0?true:false);

			if(Iter == m_Layers.rbegin())
				*pLoss += (*Iter)->GetLoss();
		}
	}

	*pLoss /= nBatchSize;

	for(auto &Layer : m_Layers)
		Layer->UpdateWeight(nBatchSize);

	return nTP;
}

void CKhuDaNet::SaveKhuDaNet(char *Filename)
{
	FILE *fp = fopen(Filename, "wb");

	if(!fp) return;

	fwrite("KhuDaNet", sizeof(char), 8, fp);

	int Cnt = m_Layers.size();
	fwrite(&Cnt, sizeof(int), 1, fp);

	for(auto &Layer : m_Layers)
	{
		fwrite(&(Layer->m_LayerOption), sizeof(CKhuDaNetLayerOption), 1, fp);
	}

	CKhuDaNetLayer *pBackwardLayer = nullptr;
	for(auto &Layer : m_Layers)
	{
		if(pBackwardLayer)
		{
			if(Layer->m_LayerOption.nLayerType & KDN_LT_FC)
			{

				if(pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
					for(int i = 0 ; i < Layer->m_LayerOption.nNodeCnt ; ++i)
						fwrite(Layer->m_Weight[i], sizeof(double), pBackwardLayer->m_LayerOption.nNodeCnt, fp);
				else if((pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
					for(int i = 0 ; i < Layer->m_LayerOption.nNodeCnt ; ++i)
						fwrite(Layer->m_Weight[i], sizeof(double),  pBackwardLayer->m_LayerOption.nImageCnt*pBackwardLayer->m_LayerOption.nW*pBackwardLayer->m_LayerOption.nH, fp);

				fwrite(Layer->m_Bias, sizeof(double), Layer->m_LayerOption.nNodeCnt, fp);
			}
			else if(Layer->m_LayerOption.nLayerType & KDN_LT_CON)
			{
				if((pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
					for(int i = 0 ; i < Layer->m_LayerOption.nImageCnt ; ++i)
						for(int j = 0 ; j < pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
							fwrite(Layer->m_CnnWeight[i][j][0], sizeof(double),  Layer->m_LayerOption.nKernelSize*Layer->m_LayerOption.nKernelSize, fp);

				fwrite(Layer->m_Bias, sizeof(double), Layer->m_LayerOption.nImageCnt, fp);
			}
		}

		pBackwardLayer = Layer;
	}

	fclose(fp);
}

void CKhuDaNet::LoadKhuDaNet(char *Filename)
{
	ClearAllLayers();

	FILE *fp = fopen(Filename, "rb");

	if(!fp) return;
	
	char Buf[10];
	int nLayerCnt;

	fread(Buf, sizeof(char), 8, fp);

	fread(&nLayerCnt, sizeof(int), 1, fp);

	for(int s = 0 ; s < nLayerCnt ; ++s)
	{
		CKhuDaNetLayer *pLayer;
		char *pRawLayerOption = new char[sizeof(CKhuDaNetLayerOption)];

		fread(pRawLayerOption, sizeof(CKhuDaNetLayerOption), 1, fp);

		CKhuDaNetLayerOption KhuDaNetLayerOption(*(CKhuDaNetLayerOption *)pRawLayerOption);

		if(s == 0) 
		{
			pLayer = new CKhuDaNetLayer(KhuDaNetLayerOption, nullptr);
			m_nInputSize = pLayer->m_LayerOption.nNodeCnt;
		}
		else
			pLayer = new CKhuDaNetLayer(KhuDaNetLayerOption, m_Layers[m_Layers.size()-1]);

		m_Layers.push_back(pLayer);
		m_nOutputSize = pLayer->m_LayerOption.nNodeCnt;

		delete [] pRawLayerOption;
	}

	CKhuDaNetLayer *pBackwardLayer = nullptr;
	for(int s = 0 ; s < nLayerCnt ; ++s)
	{
		if(pBackwardLayer)
		{
			if(m_Layers[s]->m_LayerOption.nLayerType & KDN_LT_FC)
			{
				if(pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_FC)
					for(int i = 0 ; i < m_Layers[s]->m_LayerOption.nNodeCnt ; ++i)
						fread(m_Layers[s]->m_Weight[i], sizeof(double), pBackwardLayer->m_LayerOption.nNodeCnt, fp);
				else if((pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
					for(int i = 0 ; i < m_Layers[s]->m_LayerOption.nNodeCnt ; ++i)
						fread(m_Layers[s]->m_Weight[i], sizeof(double),  pBackwardLayer->m_LayerOption.nImageCnt*pBackwardLayer->m_LayerOption.nW*pBackwardLayer->m_LayerOption.nH, fp);

				fread(m_Layers[s]->m_Bias, sizeof(double), m_Layers[s]->m_LayerOption.nNodeCnt, fp);
			}
			else if(m_Layers[s]->m_LayerOption.nLayerType & KDN_LT_CON)
			{
				if((pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_CON) || (pBackwardLayer->m_LayerOption.nLayerType & KDN_LT_POOL))
					for(int i = 0 ; i < m_Layers[s]->m_LayerOption.nImageCnt ; ++i)
						for(int j = 0 ; j < pBackwardLayer->m_LayerOption.nImageCnt ; ++j)
							fread(m_Layers[s]->m_CnnWeight[i][j][0], sizeof(double),  m_Layers[s]->m_LayerOption.nKernelSize*m_Layers[s]->m_LayerOption.nKernelSize, fp);

				fread(m_Layers[s]->m_Bias, sizeof(double), m_Layers[s]->m_LayerOption.nImageCnt, fp);
			}
		}

		pBackwardLayer = m_Layers[s];
	}

	fclose(fp);
}

int CKhuDaNet::ArgMax(double *List, int nCnt)
{
	int MaxPos = 0;

	for(int i = 0 ; i < nCnt ; ++i)
		if(List[i] > List[MaxPos]) MaxPos = i;

	return MaxPos;
}

double **CKhuDaNet::dmatrix(int nH, int nW) 
{
	double **Temp;

	Temp = new double *[nH];
	for(int y = 0 ; y < nH ; ++y)
		Temp[y] = new double[nW];

	return Temp;
}

void CKhuDaNet::free_dmatrix(double **Image, int nH, int nW)
{
	for(int y = 0 ; y < nH ; ++y)
		delete [] Image[y];

	delete [] Image;
}

double **CKhuDaNet::dmatrix1d(int nH, int nW) 
{
	double **Temp;

	Temp = new double *[nH];

	Temp[0] = new double [nH*nW];
	for(int y = 1 ; y < nH ; ++y)
		Temp[y] = Temp[0] + nW*y;

	return Temp;
}

void CKhuDaNet::free_dmatrix1d(double **Image, int nH, int nW)
{
	delete [] Image[0];

	delete [] Image;
}

double CKhuDaNet::Identify(double x)
{
	return x;
}

double CKhuDaNet::DifferentialIdentify(double x)
{
	return 1;
}

double CKhuDaNet::BinaryStep(double x)
{
	return (x>0)?1:0;
}

double CKhuDaNet::DifferentialBinaryStep(double x)
{
	return 0;
}

double CKhuDaNet::Sigmoid(double x)
{
	return 1./(1.+exp(-1. * x));
}

double CKhuDaNet::DifferentialSigmoid(double x)
{
	return x*(1.-x);
}

double CKhuDaNet::TanH(double x)
{
	return (exp(x)-exp(-x))/(exp(x)+exp(-x));
}

double CKhuDaNet::DifferentialTanH(double x)
{
	return 1-x*x;
}

double CKhuDaNet::Relu(double x)
{
	return (x > 0)?x:0;
}

double CKhuDaNet::DifferentialRelu(double x)
{
	return (x > 0)?1:0;
}

double CKhuDaNet::LeakyRelu(double x)
{
	return (x > 0)?x:0.001*x;
}

double CKhuDaNet::DifferentialLeakyRelu(double x)
{
	return (x > 0)?1:0.001;
}

double CKhuDaNet::Softmax(double x)
{
	return x;
}

double CKhuDaNet::DifferentialSoftmax(double x)
{
	return 1;
}