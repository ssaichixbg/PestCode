#pragma once

#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <vector>
#include <assert.h>

#pragma warning(disable:4996)
#define cimg_display 0
#include "CImg.h"
typedef unsigned char BYTE;

using namespace std;

class GrayData
{
public:
	GrayData() throw() : m_iW(0), m_iH(0)
	{
	}
	~GrayData() throw()
	{
	}

	void Clear() throw()
	{
		m_buffer.clear();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return m_buffer.empty();
	}

	vector<BYTE>::iterator GetAddress() throw()
	{
		return m_buffer.begin();
	}

	int GetWidth() const throw()
	{
		return m_iW;
	}
	int GetHeight() const throw()
	{
		return m_iH;
	}

	void Allocate(int iW, int iH)
	{
		assert(iW > 0 && iH > 0);
		Clear();
		m_iW = iW;
		m_iH = iH;
		m_buffer.resize(m_iW*m_iH);
	}

	void Invert() throw()
	{
		vector<BYTE>::iterator pd = m_buffer.begin();
		while (pd != m_buffer.end())
		{
			*pd = 255 - *pd;
			pd++;
		}
	}

	void BoolInvert() throw()
	{
		vector<BYTE>::iterator pd = m_buffer.begin();
		while (pd != m_buffer.end())
		{
			if (*pd != 0)
				*pd = 0;
			else
				*pd = 255;
			pd++;
		}
	}

	//copy
	void CopyTo(GrayData& dest)
	{
		if (IsNull()) 
		{
			dest.Clear();
			return;
		}
		dest.Allocate(m_iW, m_iH);
		for (int i = 0; i < m_buffer.size(); i++)
		{
			dest.m_buffer[i] = m_buffer[i];
		}
	}

private:
	vector<BYTE>m_buffer;//颜色表
	int m_iW, m_iH;//宽度、高度
};

class ColorData
{
public:
	ColorData() throw() : m_iW(0), m_iH(0)
	{
	}
	ColorData(vector<BYTE>&t_R, vector<BYTE>&t_G, vector<BYTE>&t_B, int width, int height)
	{
		R = t_R;
		G = t_G;
		B = t_B;
		m_iW = width;
		m_iH = height;
	}
	~ColorData() throw()
	{
	}

	void Clear() throw()
	{
		R.clear();
		G.clear();
		B.clear();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return R.empty() || G.empty() || B.empty();
	}

	vector<BYTE>::iterator GetAddressR() throw()
	{
		return R.begin();
	}

	vector<BYTE>::iterator GetAddressG() throw()
	{
		return G.begin();
	}

	vector<BYTE>::iterator GetAddressB() throw()
	{
		return B.begin();
	}

	int GetWidth() const throw()
	{
		return m_iW;
	}
	int GetHeight() const throw()
	{
		return m_iH;
	}

private:
	vector<BYTE>R;
	vector<BYTE>G;
	vector<BYTE>B;
	int m_iW, m_iH;
};

// ImageDataHelper

class ImageDataHelper
{
public:
	//将图像转为ColorData
	//void ImageToColorData(char *filename, unsigned char **data, int *width, int *height, vector<BYTE>&R, vector<BYTE>&G, vector<BYTE>&B)
	static void ImageToColorData(char *filename, int *width, int *height, vector<BYTE>&R, vector<BYTE>&G, vector<BYTE>&B)
	{
		cimg_library::CImg<unsigned char> image(filename);
		*width = image.width();
		*height = image.height();

		//cout << *height << endl << *width;
		cimg_forXY(image, x, y) {
			R.push_back(image(x, y, 0));
			G.push_back(image(x, y, 1));
			B.push_back(image(x, y, 2));
		}
	}
	
	//ColorData->GrayData
	static void ColorDataToGrayData(ColorData& cData, GrayData& gData)
	{
		gData.Clear();
		if (cData.IsNull())
			return;

		int height = cData.GetHeight();
		int width = cData.GetWidth();
		gData.Allocate(width, height);
		
		vector<BYTE>::iterator psR = cData.GetAddressR();
		vector<BYTE>::iterator psG = cData.GetAddressG();
		vector<BYTE>::iterator psB = cData.GetAddressB();
		vector<BYTE>::iterator pd = gData.GetAddress();
	
		for (int i = 0; i < height*width; i++)
		{
			double t_psR = (double)(*psR++);
			double t_psG = (double)(*psG++);
			double t_psB = (double)(*psB++);
			double t_pd = 0.11*t_psR + 0.59*t_psG + 0.3*t_psB;
			*(pd++) = (BYTE)((t_pd>255.0) ? 255.0 : ((t_pd<0.0) ? 0.0 : t_pd));
		}
	}
};
