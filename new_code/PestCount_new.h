////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include "ImageData_new.h"
#include "ImageProcess_new.h"

////////////////////////////////////////////////////////////////////////////////

class PestCountHelper
{
public:
	//name of method should be capital
	static int CalcPestCount(ColorData& cData, GrayData& gData)
	{
		// color->gray
		ImageDataHelper::ColorDataToGrayData(cData, gData);

		//histogram
		vector<double> histogram(256, 0.0);
		ImageProcessHelper::CalcHistogram(gData, histogram);
		//�õ���һ����ֵ
		int iFirstThreshold = ImageProcessHelper::CalcThresholdByHistogram(histogram);
		//��ֵ��
		ImageProcessHelper::GrayDataToBinaryData(iFirstThreshold, gData);
		//�ȸ���һ��
		GrayData orgData;
		gData.CopyTo(orgData);

		//�����ͨ����
		vector<vector<int> > mexConnection;
		int num = ImageProcessHelper::Label(gData, mexConnection);
		//�������
		vector<int> vecArea;
		ImageProcessHelper::CalcArea(num, mexConnection, vecArea);
		//�����������ı��
		int iLabel = 0;
		int area = 0;
		for (int i = 1; i <= num; i++) {
			if (vecArea[i] > area) {
				area = vecArea[i];
				iLabel = i;
			}
		}
		//����������� ����ֽ����,��Χ�ܶ��ɫС�����ڸ���
		ImageProcessHelper::RemoveSmallArea(mexConnection, iLabel, gData);
		//gData��ֽ�ڲ��ĺ�ɫ�Ѿ��ǲ�׼ȷ�ˣ�����Ҫ�Ѱ�ֽ���ܵ�����ǳ���
		//�ȷ�ɫ
		gData.Invert();
		//��������������ͨ����ֻʣ��һ����ɫ���򣬷�ɫ��ֻʣ��һ����ɫ������Ҫ����Χ�İ�ɫ�����Ϊ��ɫ
		vector<vector<int> > mexBorder;
		ImageProcessHelper::LabelBorder(gData, mexBorder);

		//��orgData��������������,��Ϊ��ɫ
		ImageProcessHelper::FillBorder(mexBorder, orgData);

		//���Ƹ��������
		orgData.CopyTo(gData);

		//������ɫ�����沿��Ϊ��ɫ
		orgData.BoolInvert();
		//��Ǻ��沿��
		num = ImageProcessHelper::Label(orgData, mexConnection);

		//�����Ǽ��㺦�沿��
		ImageProcessHelper::CalcArea(num, mexConnection, vecArea);
		//�����������ı��
		iLabel = 0;
		area = 0;
		for (int i = 1; i <= num; i++) {
			if (vecArea[i] > area) {
				area = vecArea[i];
				iLabel = i;
			}
		}
		//ͳ������Ĵ�С
		int areaSum = 0;
		for (int i = 1; i <vecArea.size(); i++)
		{
			areaSum += vecArea[i];
		}
		double aveArea = areaSum*1.0 / num;

		//������������ĳ��ֵ()������Ϊ�������һ�𣬴�ʱʹ��ĳ����ֵ(312)���������
		const int c_maxArea = 10000;
		const int c_minArea = 30;
		if (area > c_maxArea)
		{
			num = areaSum / c_minArea;
		}


		//ofstream outfile;
		//outfile.open("e:\\myfile.txt",ios::app);
		//outfile << "areaSum:" << areaSum << " " << "num:" << num << " " << "aveArea:" << aveArea << "max area:" << area << endl;
		//number
		return num;
	}
};

////////////////////////////////////////////////////////////////////////////////
