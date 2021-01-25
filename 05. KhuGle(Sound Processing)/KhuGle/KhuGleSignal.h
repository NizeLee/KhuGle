//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once

typedef struct  tagWAV_HEADER_
{
	char chunkID[4];			//"RIFF" = 0x46464952
	unsigned long chunkSize;	//28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char format[4];				//"WAVE" = 0x45564157
	char subchunk1ID[4];		//"fmt " = 0x20746D66
	unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
} WAV_HEADER_;

typedef struct tagCHUCK_
{
	char ID[4]; //"data" = 0x61746164
	unsigned long size;  //Chunk data bytes
} CHUCK_;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER_ {
	unsigned short		bfType;
	unsigned long		bfSize;
	unsigned short		bfReserved1;
	unsigned short		bfReserved2;
	unsigned long		bfOffBits;
} BITMAPFILEHEADER_;

typedef struct tagBITMAPINFOHEADER_ {
	unsigned long       biSize;
	long				biWidth;
	long				biHeight;
	unsigned short      biPlanes;
	unsigned short      biBitCount;
	unsigned long       biCompression;
	unsigned long       biSizeImage;
	long				biXPelsPerMeter;
	long				biYPelsPerMeter;
	unsigned long       biClrUsed;
	unsigned long       biClrImportant;
} BITMAPINFOHEADER_;

typedef struct tagRGBQUAD_ {
	unsigned char    rgbBlue;
	unsigned char    rgbGreen;
	unsigned char    rgbRed;
	unsigned char    rgbReserved;
} RGBQUAD_;
#pragma pack(pop)

#define BI_RGB_        0L

class CKhuGleSignal
{
public:
	short int *m_Samples;
	int m_nSampleRate;
	int m_nSampleLength;

	double **m_Real, **m_Imaginary;
	int m_nWindowSize;
	int m_nFrequencySampleLength;

	int m_nW, m_nH;
	unsigned char **m_Red, **m_Green, **m_Blue;

	CKhuGleSignal();
	~CKhuGleSignal();

	void ReadWave(char *FileName);
	bool SaveWave(char *FileName);

	void ReadBmp(char *FileName);
	bool SaveBmp(char *FileName);
	
	void MakeSpectrogram();
};

