//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once
#include "KhuDaNetLayer.h"
#include <vector>

#define MAX_INFORMATION_STRING_SIZE	1000

class CKhuDaNet
{
public:
	CKhuDaNet();
	virtual ~CKhuDaNet();

	std::vector<CKhuDaNetLayer*> m_Layers;

	int m_nInputSize, m_nOutputSize;
	char *m_Information;

	char *GetInformation();
	bool IsNetwork();
	void ClearAllLayers();
	void AddLayer(CKhuDaNetLayer *pLayer);
	void AddLayer(CKhuDaNetLayerOption LayerOptionInput);
	void AllocDeltaWeight();
	void InitWeight();
	int Forward(double *Input, double *Probability = 0);
	int TrainBatch(double **Input, double **Output, int nBatchSize, double *pLoss);
	void SaveKhuDaNet(char *Filename);
	void LoadKhuDaNet(char *Filename);

	static int ArgMax(double *List, int nCnt);
	static double **dmatrix(int nH, int nW);
	static void free_dmatrix(double **Image, int nH, int nW);
	static double **dmatrix1d(int nH, int nW);
	static void free_dmatrix1d(double **Image, int nH, int nW);
	static double Identify(double x);
	static double DifferentialIdentify(double x);
	static double BinaryStep(double x);
	static double DifferentialBinaryStep(double x);
	static double Sigmoid(double x);
	static double DifferentialSigmoid(double x);
	static double TanH(double x);
	static double DifferentialTanH(double x);
	static double Relu(double x);
	static double DifferentialRelu(double x);
	static double LeakyRelu(double x);
	static double DifferentialLeakyRelu(double x);
	static double Softmax(double x);
	static double DifferentialSoftmax(double x);
};

