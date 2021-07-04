//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once

#define KDN_LT_FC			0x0001

#define KDN_LT_INPUT		0x0100
#define KDN_LT_HIDDEN		0x0200
#define KDN_LT_OUTPUT		0x0400

#define KDN_AF_NONE				0
#define KDN_AF_IDENTIFY			1
#define KDN_AF_BINARY_STEP		2
#define KDN_AF_SIGMOID			3
#define KDN_AF_TANH				4
#define KDN_AF_RELU				5
#define KDN_AF_LEAKY_RELU		6
#define KDN_AF_SOFTMAX			7

struct CKhuDaNetLayerOption{
	CKhuDaNetLayerOption(unsigned int nLayerTypeIntput, int nImageCntInput, int nNodeCntIput, 
		int nWidthInput, int nHeightInput, int nKernelSizeInput, 
		int nActicationFnInput, double dLearningRateInput);
	unsigned int nLayerType;
	int nImageCnt;
	int nNodeCnt;
	int nW, nH;
	int nKernelSize;
	int nActicationFn;

	double dLearningRate;
};

class CKhuDaNetLayer
{
public:
	CKhuDaNetLayerOption m_LayerOption;
	CKhuDaNetLayer *m_pBackwardLayer;
	bool m_bTrained;
	
	double *m_Node;
	double **m_Weight;
	
	double *m_Bias;

	double *m_Loss;

	double *m_DeltaNode;
	double **m_DeltaWeight;
	
	double *m_DeltaBias;

	double (*Activation)(double);
	double (*DifferentialActivation)(double);

	CKhuDaNetLayer(CKhuDaNetLayerOption m_LayerOptionInput, CKhuDaNetLayer *pBackwardLayerInput);
	virtual ~CKhuDaNetLayer();

	void AllocDeltaWeight();
	void InitWeight();
	int ComputeLayer(double *Probability = 0);
	void ComputeDelta(double *Output);
	void ComputeDeltaWeight(bool bReset);
	void UpdateWeight(int nBatchSize);
	double GetLoss();
};

