////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

// ImageProcessHelper

class ImageProcessHelper
{
public:
	//由灰度图得到直方图
	static void CalcHistogram(const GrayData& gData, vector<double>& histogram) throw()
	{
		ATLASSERT( histogram.size() == 256 );

		int height = gData.GetHeight();
		int width = gData.GetWidth();
		int num = height*width;//总像素数

		//直方图大小，应为256
		int bins = (int)histogram.size();
		for (int i = 0; i < bins; i++) {
			histogram[i] = 0.0;
		}

		BYTE* ps = gData.GetAddress();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int t_pd = (int)(*ps++);
				histogram[t_pd] ++; //建立直方图
			}
		}

		for (int i = 0; i < bins; i++)
		{
			histogram[i] /= num;//频数化为频率
		}
	}

	//由直方图得到阈值,ostu
	static int CalcThresholdByHistogram(const vector<double>& histogram) throw()
	{
		ATLASSERT( histogram.size() == 256 );

		//直方图大小，应为256
		int iBins = (int)histogram.size();

		double variance[256];//类间方差
		double pa = 0.0;//背景出现概率
		double pb = 0.0;//目标出现概率
		double wa = 0.0;//背景平均灰度值
		double wb = 0.0;//目标平均灰度值
		double w0 = 0.0;//全局平均灰度值
		double dData1 = 0.0, dData2 = 0.0;

		memset(variance, 0, sizeof(variance));

		//计算全局平均灰度值
		for (int i = 0; i < iBins; i++)
		{
			w0 += i*histogram[i];
		}
		//对每个灰度值计算类间方差
		for (int i = 0; i < iBins; i++)
		{
			pa += histogram[i];
			pb = 1.0 - pa;
			dData1 += i*histogram[i];
			dData2 = w0 - dData1;
			wa = dData1 / pa;
			wb = dData2 / pb;
			variance[i] = pa*(wa - w0)*(wa - w0) + pb*(wb - w0)*(wb - w0);
		}
		//遍历，得到类间最大方差对应的灰度值
		double temp = 0.0;
		int threshold = 0;
		for (int i = 0; i < iBins; i++)
		{
			if (variance[i] > temp)
			{
				temp = variance[i];
				threshold = i;
			}
		}

		return threshold;
	}

	//计算第二个阈值
	// 先将其拷贝到新的vector中，得到新的阈值
	static int CalcHighThresholdByHistogram(const vector<double>& histogram, int iFirstThreshold)
	{
		ATLASSERT( histogram.size() == 256 );

		vector<double> newHistogram(256, 0.0);
		//直方图大小，应为256
		int iBins = (int)histogram.size();

		double sum = 0.0;
		for (int i = iFirstThreshold; i < iBins; i++)
		{
			sum += histogram[i];
		}
		for (int i = iFirstThreshold; i < iBins; i++)
		{
			newHistogram[i] = histogram[i] / sum;
		}
		return CalcThresholdByHistogram(newHistogram);
	}

	//然后对于大于阈值的pixel重新处理，将灰度图变为二值图
	static void GrayDataToBinaryData(int iThreshold, GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int temp = (int)(*pd);
				
				//if (temp >= secondThreshold)
				if (temp>=iThreshold)
				{
					*pd = (BYTE)255;
				}
				else
				{
					*pd = (BYTE)0;
				}
				pd++;
			}

		}
	}

	//连通域标记算法,标记出连通域
	//给出种子点的标记
	static void LabelGrow(int iLabel, int x, int y, GrayData& gData, vector<vector<int>>& matrix)
	{
		BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		stack<pair<int, int>> coordinate_stack;
		coordinate_stack.push(make_pair(y, x));
		matrix[y][x] = iLabel;

		int t_ps;
		//区域增长,白色，没有标记过
		while (!coordinate_stack.empty())
		{
			pair<int, int>t_coordinate = coordinate_stack.top();
			coordinate_stack.pop();
			int t_y = t_coordinate.first;  //height, y
			int t_x = t_coordinate.second; //width, x
			int n_y;
			int n_x;

			n_y = t_y;
			//左边
			n_x = t_x - 1;
			if (n_x >= 0 )
			{
				t_ps = (int)(*(ps + n_y*width + n_x));
				if (t_ps != 0 && matrix[n_y][n_x] == 0)
				{
					matrix[n_y][n_x] = iLabel;
					coordinate_stack.push(make_pair(n_y, n_x));
				}
			}
			//右边
			n_x = t_x + 1;
			if (n_x < width)
			{
				t_ps = (int)(*(ps + n_y*width + n_x));
				if (t_ps != 0 && matrix[n_y][n_x] == 0)
				{
					matrix[n_y][n_x] = iLabel;
					coordinate_stack.push(make_pair(n_y, n_x));
				}
			}
			n_x = t_x;
			//上边
			n_y = t_y - 1;
			if (n_y >= 0)
			{
				t_ps = (int)(*(ps + n_y*width + n_x));
				if (t_ps != 0 && matrix[n_y][n_x] == 0)
				{
					matrix[n_y][n_x] = iLabel;
					coordinate_stack.push(make_pair(n_y, n_x));
				}
			}
			//下边
			n_y = t_y + 1;
			if (n_y < height)
			{
				t_ps = (int)(*(ps + n_y*width + n_x));
				if (t_ps != 0 && matrix[n_y][n_x] == 0)
				{
					matrix[n_y][n_x] = iLabel;
					coordinate_stack.push(make_pair(n_y, n_x));
				}
			}
		}
	}

	//所有都标记
	static int Label(GrayData& gData, vector<vector<int>>& matrix)
	{
		BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		matrix.resize(height);
		for (int i = 0; i < height; i++)
		{
			matrix[i].resize(width);
		}

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				matrix[i][j] = 0;
			}
		}

		int t_ps;
		int label = 0;
		//执行完成后，若matrix[i][j]的值为0，则表明这块区域是黑色，否则相同标号的为同一区域
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps != 0 && matrix[i][j] == 0)
				{
					label++;
					//获得一个种子点
					LabelGrow(label, j, i, gData, matrix);
				}
			}
		}
		return label;
	}

	//得到各白色连通域的面积
	static void CalcArea(int iLabelNum, const vector<vector<int>>& matrix, vector<int>& vecArea)
	{
		//label num + 1个元素
		vecArea.resize(iLabelNum + 1);
		for (int i = 0; i <= iLabelNum; i++)
			vecArea[i] = 0;

		for (int i = 0; i < matrix.size(); i++)
		{
			for (int j = 0; j < matrix[0].size(); j++)
			{
				int v = matrix[i][j];
				if (v == 0)
					continue;
				vecArea[v] ++;
			}
		}
	}

	//得到各连通域面积大小，保留面积最大的区域，其他区域全部变为黑色
	static void RemoveSmallArea(const vector<vector<int>>& matrix, int iLabel, GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (matrix[i][j] == iLabel||matrix[i][j]==0)//面积最大区域或本身黑色区域忽略
					continue;
				int offset = i*width + j;
				*(pd + offset) = (BYTE)0;
			}
		}
	}

	//标记外周的特殊连通区域
	static int LabelBorder(GrayData& gData, vector<vector<int>>& matrix)
	{
		BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		matrix.resize(height);
		for (int i = 0; i < height; i++)
		{
			matrix[i].resize(width);
		}

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				matrix[i][j] = 0;
			}
		}

		int t_ps;
		int label = 0;
		//只考虑外面的白色区域，不能处理里面的白色区域
		//左上角开始
		for (int i = 0; i < height; i++)
		{
			bool bExit = false;
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//这个方位已经标记过，不用处理
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//获得一个种子点
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if (bExit)
				break;
		}
		//右上角开始
		for (int i = 0; i < height; i++)
		{
			bool bExit = false;
			for (int j = width-1; j >= 0; j--)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//这个方位已经标记过，不用处理
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//获得一个种子点
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if( bExit )
				break;
		}
		//左下角开始
		for (int i = height - 1; i >= 0; i--)
		{
			bool bExit = false;
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//这个方位已经标记过，不用处理
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//获得一个种子点
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if( bExit )
				break;
		}
		//右下角开始
		for (int i = height - 1; i >= 0; i--)
		{
			bool bExit = false;
			for (int j = width -1 ; j>=0; j--)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//这个方位已经标记过，不用处理
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//获得一个种子点
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if( bExit )
				break;
		}

		return label;
	}
	//设置外周区域
	static void FillBorder(const vector<vector<int>>& matrix, GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (matrix[i][j] != 0) {
					int offset = i*width + j;
					*(pd + offset) = (BYTE)128;
				}
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
