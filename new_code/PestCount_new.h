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
		//得到第一个阈值
		int iFirstThreshold = ImageProcessHelper::CalcThresholdByHistogram(histogram);
		//二值化
		ImageProcessHelper::GrayDataToBinaryData(iFirstThreshold, gData);
		//先复制一下
		GrayData orgData;
		gData.CopyTo(orgData);

		//标记连通区域
		vector<vector<int> > mexConnection;
		int num = ImageProcessHelper::Label(gData, mexConnection);
		//计算面积
		vector<int> vecArea;
		ImageProcessHelper::CalcArea(num, mexConnection, vecArea);
		//计算最大面积的标号
		int iLabel = 0;
		int area = 0;
		for (int i = 1; i <= num; i++) {
			if (vecArea[i] > area) {
				area = vecArea[i];
				iLabel = i;
			}
		}
		//保留最大区域 即白纸区域,周围很多白色小块属于干扰
		ImageProcessHelper::RemoveSmallArea(mexConnection, iLabel, gData);
		//gData白纸内部的黑色已经是不准确了，所以要把白纸外周单独标记出来
		//先反色
		gData.Invert();
		//标记外周特殊的连通区域，只剩下一个白色区域，反色后只剩下一个黑色区域，需要将周围的白色区域变为灰色
		vector<vector<int> > mexBorder;
		ImageProcessHelper::LabelBorder(gData, mexBorder);

		//在orgData上设置外周区域,设为灰色
		ImageProcessHelper::FillBorder(mexBorder, orgData);

		//复制给输出参数
		orgData.CopyTo(gData);

		//布尔反色，害虫部分为白色
		orgData.BoolInvert();
		//标记害虫部分
		num = ImageProcessHelper::Label(orgData, mexConnection);

		//以下是计算害虫部分
		ImageProcessHelper::CalcArea(num, mexConnection, vecArea);
		//计算最大面积的标号
		iLabel = 0;
		area = 0;
		for (int i = 1; i <= num; i++) {
			if (vecArea[i] > area) {
				area = vecArea[i];
				iLabel = i;
			}
		}
		//统计面积的大小
		int areaSum = 0;
		for (int i = 1; i <vecArea.size(); i++)
		{
			areaSum += vecArea[i];
		}
		double aveArea = areaSum*1.0 / num;

		//若最大面积大于某个值()，则认为昆虫聚在一起，此时使用某个定值(312)来计算个数
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
