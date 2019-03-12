////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////
//灰度数组
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>
#include <iterator>
#include <unordered_map>

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
		m_buffer.Free();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return m_buffer.m_p == NULL;
	}

	BYTE* GetAddress() const throw()
	{
		return m_buffer.m_p;
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
		ATLASSERT( iW > 0 && iH > 0 );
		Clear();
		if( !m_buffer.Allocate(iW * iH) )
			AtlThrow(E_OUTOFMEMORY);
		m_iW = iW;
		m_iH = iH;
	}

	void Invert() throw()
	{
		BYTE* pd = GetAddress();
		for( int i = 0; i < m_iH; i ++ ) {
			for( int j = 0; j < m_iW; j ++ ) {
				*pd = 255 - *pd;
				pd ++;
			}
		}
	}
	void BoolInvert() throw()
	{
		BYTE* pd = GetAddress();
		for( int i = 0; i < m_iH; i ++ ) {
			for( int j = 0; j < m_iW; j ++ ) {
				if( *pd != 0 )
					*pd = 0;
				else
					*pd = 255;
				pd ++;
			}
		}
	}

	//copy
	void CopyTo(GrayData& dest)
	{
		if( IsNull() ) {
			dest.Clear();
			return ;
		}
		dest.Allocate(m_iW, m_iH);
		CopyMemory(dest.GetAddress(), GetAddress(), m_iW * m_iH * sizeof(BYTE));
	}

private:
	ATL::CAutoVectorPtr<BYTE> m_buffer;//颜色表
	int m_iW, m_iH;//宽度、高度
};

class ColorData
{
public:
	ColorData() throw() : m_iW(0), m_iH(0)
	{
	}
	~ColorData() throw()
	{
	}

	void Clear() throw()
	{
		m_spR.Free();
		m_spG.Free();
		m_spB.Free();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return m_spR.m_p == NULL && m_spG.m_p == NULL && m_spB.m_p == NULL;
	}

	BYTE* GetAddressR() const throw()
	{
		return m_spR.m_p;
	}
	BYTE* GetAddressG() const throw()
	{
		return m_spG.m_p;
	}
	BYTE* GetAddressB() const throw()
	{
		return m_spB.m_p;
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
		ATLASSERT( iW > 0 && iH > 0 );
		Clear();
		if( !m_spR.Allocate(iW * iH) )
			AtlThrow(E_OUTOFMEMORY);
		if( !m_spG.Allocate(iW * iH) )
			AtlThrow(E_OUTOFMEMORY);
		if( !m_spB.Allocate(iW * iH) )
			AtlThrow(E_OUTOFMEMORY);
		m_iW = iW;
		m_iH = iH;
	}

private:
	ATL::CAutoVectorPtr<BYTE> m_spR;
	ATL::CAutoVectorPtr<BYTE> m_spG;
	ATL::CAutoVectorPtr<BYTE> m_spB;
	int m_iW, m_iH;
};

// ImageDataHelper

class ImageDataHelper
{
public:
	//CImage->ColorData
	static void ImageToColorData(CImage& image, ColorData& data)
	{
		data.Clear();
		if( image.IsNull() )
			return ;

		int iBPP = image.GetBPP();//得到单位像素的位数
		ATLASSERT( iBPP == 8 || iBPP == 24 );  //必须是灰度图或彩色图才转换
		int iW = image.GetWidth();
		int iH = image.GetHeight();
		data.Allocate(iW, iH);  //may throw

		RGBQUAD table[256];//结构数组,调色板，彩色图没有调色板
		if (iBPP==8)
			image.GetColorTable(0, 256, table);//得到图像的RGB信息
		
		BYTE* ps = (BYTE*)image.GetBits();//返回图像数据buffer指针
		BYTE* pdR = data.GetAddressR();
		BYTE* pdG = data.GetAddressG();
		BYTE* pdB = data.GetAddressB();
		
		for( int i = 0; i < iH; i ++ ) {
			BYTE* psr = ps;
			for( int j = 0; j < iW; j ++ ) {
				if( iBPP == 8 ) {//8位灰度图,每一字节是一个像素灰度级
					BYTE v = *psr ++;
					*pdR ++ = table[v].rgbRed;
					*pdG ++ = table[v].rgbGreen;
					*pdB ++ = table[v].rgbBlue;
				}
				else {//24位彩色,每三字节是一个像素RGB(存储顺序是BGR)
					*pdB ++ = *psr ++;
					*pdG ++ = *psr ++;
					*pdR ++ = *psr ++;
				}
			}
			ps += image.GetPitch();
		} //end for
	}
	//ColorData->CImage
	static void ColorDataToImage(ColorData& data, CImage& image)
	{
		image.Destroy();
		if( data.IsNull() )
			return ;
		int iW = data.GetWidth();
		int iH = data.GetHeight();
		if( !image.Create(iW, iH, 24) )
			return ;

		BYTE* psR = data.GetAddressR();
		BYTE* psG = data.GetAddressG();
		BYTE* psB = data.GetAddressB();
		BYTE* pd = (BYTE*)image.GetBits();
		for( int i = 0; i < iH; i ++ ) {
			BYTE* pdr = pd;
			for( int j = 0; j < iW; j ++ ) {
				*pdr ++ = *psB ++;
				*pdr ++ = *psG ++;
				*pdr ++ = *psR ++;
			}
			pd += image.GetPitch();
		} //end for
	}
	//GrayData->CImage
	static void GrayDataToImage(GrayData& data, CImage& image)
	{
		image.Destroy();
		if( data.IsNull() )
			return ;
		int iW = data.GetWidth();
		int iH = data.GetHeight();
		if( !image.Create(iW, iH, 8) )
			return ;

		RGBQUAD table[256];
		for( int i = 0; i < 256; i ++ ) {
			table[i].rgbRed = i;
			table[i].rgbGreen = i;
			table[i].rgbBlue = i;
			table[i].rgbReserved = 0;
		}
		image.SetColorTable(0, 256, table);

		BYTE* ps = data.GetAddress();
		BYTE* pd = (BYTE*)image.GetBits();
		for( int i = 0; i < iH; i ++ ) {
			BYTE* pdr = pd;
			for( int j = 0; j < iW; j ++ ) {
				*pdr ++ = *ps ++;
			}
			pd += image.GetPitch();
		} //end for
	}
	//ColorData->GrayData
	static void ColorDataToGrayData(const ColorData& cData,GrayData& gData)
	{
		gData.Clear();
		if( cData.IsNull() )
			return;
		
		int height = cData.GetHeight();
		int width = cData.GetWidth();
		gData.Allocate(width,height);
		
		BYTE* psR = cData.GetAddressR();
		BYTE* psG = cData.GetAddressG();
		BYTE* psB = cData.GetAddressB();
		BYTE* pd = gData.GetAddress();
		
		for(int i=0;i<height;i++)
		{
			for(int j=0;j<width;j++)
			{
				//*(pd++)=0.11*(*(psR++))+0.59*(*(psG++))+0.3*(*(psB++));
				double t_psR=(double)(*psR++);
				double t_psG=(double)(*psG++);
				double t_psB=(double)(*psB++);
				double t_pd=0.11*t_psR+0.59*t_psG+0.3*t_psB;
				*(pd++)=(BYTE)((t_pd>255.0)?255.0:((t_pd<0.0)?0.0:t_pd));
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
