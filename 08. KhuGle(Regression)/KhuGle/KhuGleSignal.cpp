//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuGleSignal.h"
#include "KhuGleBase.h"
#include <cstdio>

#pragma warning(disable:4996)

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

CKhuGleSignal::CKhuGleSignal()
{
	m_Samples = nullptr;

	m_Real = nullptr;
	m_Imaginary = nullptr;
	
	m_nWindowSize = 256;
	m_nFrequencySampleLength = 1024;

	m_Red = m_Green = m_Blue = nullptr;
}

CKhuGleSignal::~CKhuGleSignal()
{
	if(m_Samples) delete [] m_Samples;
	if(m_Real) free_dmatrix(m_Real, m_nFrequencySampleLength, m_nWindowSize);
	if(m_Imaginary) free_dmatrix(m_Imaginary, m_nFrequencySampleLength, m_nWindowSize);

	if(m_Red) free_cmatrix(m_Red, m_nH, m_nW);
	if(m_Green) free_cmatrix(m_Green, m_nH, m_nW);
	if(m_Blue) free_cmatrix(m_Blue, m_nH, m_nW);
}

void CKhuGleSignal::ReadWave(char *FileName)
{
	if(m_Samples) delete [] m_Samples;
	m_Samples = nullptr;

	FILE *fp = fopen(FileName, "rb");
	if(!fp) return;

	WAV_HEADER_ header;
	fread(&header, sizeof(header), 1, fp);
	CHUCK_ chunk;
	while (true)
	{
		fread(&chunk, sizeof(chunk), 1, fp);

		if (*(unsigned int *)&chunk.ID == 0x61746164)
			break;

		fseek(fp, chunk.size, SEEK_CUR);
	}

	if(header.audioFormat != 1) return;;
	if(header.bitsPerSample != 8 && header.bitsPerSample != 16) return;

	int nBytePerSample = header.bitsPerSample / 8;
	int nSamplesCount = chunk.size * 8 / header.bitsPerSample;

	if(m_Samples) delete [] m_Samples;
	m_Samples = new short int[nSamplesCount];
	memset(m_Samples, 0, sizeof(short int) * nSamplesCount);

	for(int i = 0; i < nSamplesCount/header.numChannels ; i++)
	{
		short int Buf;
		fread(&m_Samples[i], nBytePerSample, 1, fp);
		if(nBytePerSample == 1) m_Samples[i] = (m_Samples[i]-128)*32767/127;
		if(header.numChannels == 2)
			fread(&Buf, nBytePerSample, 1, fp);
	}

	fclose(fp);

	m_nSampleRate = header.sampleRate;
	m_nSampleLength = nSamplesCount/header.numChannels;

	header.byteRate /= header.numChannels;
	header.blockAlign /= header.numChannels;
	header.numChannels = 1;
}

bool CKhuGleSignal::SaveWave(char *FileName)
{
	if(m_Samples == nullptr) return false;

	WAV_HEADER_ waveheader = {
		{'R','I','F','F'}, 0,
		{'W','A','V','E'},
		{'f','m','t',' '}, 16L,
		1,			//* 1=PCM 
		1,			//* 1=mono, 2=stereo 
		0,			//* Samples per Second 
		0,			//* Average Bytes per Second 
		1,			//* Bytes per Sample 
		16,			//* Bits per Sample 
	};

	CHUCK_ chuck = {{'d','a','t','a'}, 0};

	waveheader.chunkSize = 16L + m_nSampleLength*2 + 20L;
	waveheader.sampleRate = m_nSampleRate;
	waveheader.byteRate = m_nSampleRate*2;
	chuck.size = m_nSampleLength*2;

	FILE *fp;	
	fp = fopen(FileName,"wb");
	if(!fp) return false;


	fwrite(&(waveheader), 1, sizeof(WAV_HEADER_), fp);
	fwrite(&(chuck), 1, sizeof(CHUCK_), fp);
	fwrite(m_Samples, sizeof(short int), m_nSampleLength, fp);
	fclose(fp);

	return true;
} 

void CKhuGleSignal::ReadBmp(char *FileName)
{
	if(m_Red) free_cmatrix(m_Red, m_nH, m_nW);
	if(m_Green) free_cmatrix(m_Green, m_nH, m_nW);
	if(m_Blue) free_cmatrix(m_Blue, m_nH, m_nW);

	m_Red = m_Green = m_Blue = nullptr;

	BITMAPFILEHEADER_ bmiFileHeader;
	BITMAPINFOHEADER_ bmiHeader;

	FILE *fp;
	int nColors;

	fp = fopen(FileName, "rb");

	if(!fp) return;

	fread(&bmiFileHeader, sizeof(BITMAPFILEHEADER_), 1, fp);

	if(bmiFileHeader.bfType != 'M'*0x0100 + 'B') {
		fclose(fp);
		return;
	}

	fread(&bmiHeader, sizeof(BITMAPINFOHEADER_), 1, fp);

	if(bmiHeader.biCompression != BI_RGB_) {
		fclose(fp);
		return;
	}

	if(bmiHeader.biClrUsed == 0) 
		nColors = 1 << bmiHeader.biBitCount;
	else
		nColors = bmiHeader.biClrUsed;

	RGBQUAD_ *Palette = nullptr;

	switch (bmiHeader.biBitCount) {
	case 24:
		break;

	case 1:
	case 4:
	case 8:
		Palette = new RGBQUAD_[nColors];

		int i;
		for(i = 0 ; i < nColors ; i++) {
			unsigned char r, g, b, temp;

			fread(&b, sizeof(unsigned char), 1, fp);
			fread(&g, sizeof(unsigned char), 1, fp);
			fread(&r, sizeof(unsigned char), 1, fp);
			fread(&temp, sizeof(unsigned char), 1, fp);

			Palette[i].rgbRed = r;
			Palette[i].rgbGreen = g;
			Palette[i].rgbBlue = b;
		}
		break;
	}

	fseek(fp, bmiFileHeader.bfOffBits, SEEK_SET);

	m_nW = bmiHeader.biWidth;
	m_nH = bmiHeader.biHeight;

	m_Red = cmatrix(m_nH, m_nW);
	m_Green = cmatrix(m_nH, m_nW);
	m_Blue = cmatrix(m_nH, m_nW);

	unsigned char *Image1D = nullptr;

	Image1D = new unsigned char[m_nH*((m_nW*3+3)/4*4)];

	if(bmiHeader.biBitCount == 24)
		fread(Image1D, sizeof(unsigned char), m_nH*((m_nW*3+3)/4*4), fp);

	long Row, Col;

	for(Row = 0 ; Row < bmiHeader.biHeight ; Row++)
	{
		if(bmiHeader.biBitCount != 24) 
		{
			int BitCount = 0;
			unsigned int mask = (1 << bmiHeader.biBitCount) - 1;

			unsigned char ReadByte = 0;
			int ReadByteCnt = 0;

			for(Col = 0; Col < m_nW ; Col++) 
			{				
				int PaletteIndex = 0;

				if (BitCount <= 0) {
					BitCount = 8;
					fread(&ReadByte, sizeof(unsigned char), 1, fp);
					ReadByteCnt++;
				}

				BitCount -= bmiHeader.biBitCount;

				PaletteIndex = (ReadByte >> BitCount) & mask;

				int Pos;

				Pos = (((m_nW*3+3)/4*4) * Row) + Col*3;	
				Image1D[Pos++] = Palette[PaletteIndex].rgbBlue;
				Image1D[Pos++] = Palette[PaletteIndex].rgbGreen;
				Image1D[Pos] = Palette[PaletteIndex].rgbRed;
			}

			while (ReadByteCnt&3) 
			{
				char temp;
				fread(&temp, sizeof(char), 1, fp);
				ReadByteCnt++;
			}
		}
	}

	if(Palette) delete [] Palette;

	fclose(fp);

	for(int y = 0 ; y < m_nH ; y++)
		for(int x = 0 ; x < m_nW ; x++)
		{
			int pos = (m_nW*3+3)/4*4*(m_nH-y-1) + x*3;

			m_Red[y][x] = Image1D[pos+2];
			m_Green[y][x] = Image1D[pos+1];
			m_Blue[y][x] = Image1D[pos];
		}

	delete [] Image1D;
}

bool CKhuGleSignal::SaveBmp(char *FileName)
{
	if(m_Red == nullptr) return false;
	if(m_Green == nullptr) return false;
	if(m_Blue == nullptr) return false;

	unsigned char *Image1D = new unsigned char[m_nH*((m_nW*3+3)/4*4)];

	for(int y = 0 ; y < m_nH ; y++)
		for(int x = 0 ; x < m_nW ; x++)
		{
			int pos = (m_nW*3+3)/4*4*(m_nH-y-1) + x*3;

			Image1D[pos+2] = m_Red[y][x];
			Image1D[pos+1] = m_Green[y][x];
			Image1D[pos] = m_Blue[y][x];
		}

	unsigned long dwBitsSize;

	unsigned long widthDW = (m_nW*3+3)/4*4;;
	unsigned long size = widthDW*m_nH;
	dwBitsSize = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_) + size;

	BITMAPINFOHEADER_ bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER_);
	bmiHeader.biWidth = m_nW;
	bmiHeader.biHeight = m_nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB_;
	bmiHeader.biSizeImage = size;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER_ bmiFileHeader;
	bmiFileHeader.bfType = 'M'*0x0100 + 'B';
	bmiFileHeader.bfSize = dwBitsSize;
	bmiFileHeader.bfReserved1 = 0;
	bmiFileHeader.bfReserved2 = 0;
	bmiFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_);

	FILE *fp;

	fp = fopen(FileName, "wb");
	if(!fp) return false;

	fwrite(&bmiFileHeader, 1, sizeof(BITMAPFILEHEADER_), fp);
	fwrite(&bmiHeader, 1, sizeof(BITMAPINFOHEADER_), fp);
	fwrite(Image1D, 1, size, fp);
	fclose(fp);

	delete [] Image1D;

	return true;
}

void CKhuGleSignal::MakeSpectrogram()
{
	if(!m_Real) m_Real = dmatrix(m_nFrequencySampleLength, m_nWindowSize);
	if(!m_Imaginary) m_Imaginary = dmatrix(m_nFrequencySampleLength, m_nWindowSize);

	double *OrgReal = new double[m_nWindowSize];
	double *OrgImaginary = new double[m_nWindowSize];

	for(int t = 0 ; t < m_nFrequencySampleLength ; t++)
	{
		int OrgT = t*m_nSampleLength/m_nFrequencySampleLength;
		for(int dt = 0 ; dt < m_nWindowSize ; dt++)
		{
			int tt = OrgT+dt-m_nWindowSize/2;
			if(tt >= 0 && tt < m_nSampleLength)
				OrgReal[dt] = m_Samples[tt];
			else
				OrgReal[dt] = 0;
			OrgImaginary[dt] = 0;
		}

		FFT2Radix(OrgReal, OrgImaginary, m_Real[t], m_Imaginary[t], m_nWindowSize, false);
	}

	delete [] OrgReal;
	delete [] OrgImaginary;
}