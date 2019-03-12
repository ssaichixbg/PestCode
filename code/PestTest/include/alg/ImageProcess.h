////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

// ImageProcessHelper

class ImageProcessHelper
{
public:
	//�ɻҶ�ͼ�õ�ֱ��ͼ
	static void CalcHistogram(const GrayData& gData, vector<double>& histogram) throw()
	{
		ATLASSERT( histogram.size() == 256 );

		int height = gData.GetHeight();
		int width = gData.GetWidth();
		int num = height*width;//��������

		//ֱ��ͼ��С��ӦΪ256
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
				histogram[t_pd] ++; //����ֱ��ͼ
			}
		}

		for (int i = 0; i < bins; i++)
		{
			histogram[i] /= num;//Ƶ����ΪƵ��
		}
	}

	//��ֱ��ͼ�õ���ֵ,ostu
	static int CalcThresholdByHistogram(const vector<double>& histogram) throw()
	{
		ATLASSERT( histogram.size() == 256 );

		//ֱ��ͼ��С��ӦΪ256
		int iBins = (int)histogram.size();

		double variance[256];//��䷽��
		double pa = 0.0;//�������ָ���
		double pb = 0.0;//Ŀ����ָ���
		double wa = 0.0;//����ƽ���Ҷ�ֵ
		double wb = 0.0;//Ŀ��ƽ���Ҷ�ֵ
		double w0 = 0.0;//ȫ��ƽ���Ҷ�ֵ
		double dData1 = 0.0, dData2 = 0.0;

		memset(variance, 0, sizeof(variance));

		//����ȫ��ƽ���Ҷ�ֵ
		for (int i = 0; i < iBins; i++)
		{
			w0 += i*histogram[i];
		}
		//��ÿ���Ҷ�ֵ������䷽��
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
		//�������õ������󷽲��Ӧ�ĻҶ�ֵ
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

	//����ڶ�����ֵ
	// �Ƚ��俽�����µ�vector�У��õ��µ���ֵ
	static int CalcHighThresholdByHistogram(const vector<double>& histogram, int iFirstThreshold)
	{
		ATLASSERT( histogram.size() == 256 );

		vector<double> newHistogram(256, 0.0);
		//ֱ��ͼ��С��ӦΪ256
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

	//Ȼ����ڴ�����ֵ��pixel���´������Ҷ�ͼ��Ϊ��ֵͼ
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

	//��ͨ�����㷨,��ǳ���ͨ��
	//�������ӵ�ı��
	static void LabelGrow(int iLabel, int x, int y, GrayData& gData, vector<vector<int>>& matrix)
	{
		BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		stack<pair<int, int>> coordinate_stack;
		coordinate_stack.push(make_pair(y, x));
		matrix[y][x] = iLabel;

		int t_ps;
		//��������,��ɫ��û�б�ǹ�
		while (!coordinate_stack.empty())
		{
			pair<int, int>t_coordinate = coordinate_stack.top();
			coordinate_stack.pop();
			int t_y = t_coordinate.first;  //height, y
			int t_x = t_coordinate.second; //width, x
			int n_y;
			int n_x;

			n_y = t_y;
			//���
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
			//�ұ�
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
			//�ϱ�
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
			//�±�
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

	//���ж����
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
		//ִ����ɺ���matrix[i][j]��ֵΪ0���������������Ǻ�ɫ��������ͬ��ŵ�Ϊͬһ����
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps != 0 && matrix[i][j] == 0)
				{
					label++;
					//���һ�����ӵ�
					LabelGrow(label, j, i, gData, matrix);
				}
			}
		}
		return label;
	}

	//�õ�����ɫ��ͨ������
	static void CalcArea(int iLabelNum, const vector<vector<int>>& matrix, vector<int>& vecArea)
	{
		//label num + 1��Ԫ��
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

	//�õ�����ͨ�������С�������������������������ȫ����Ϊ��ɫ
	static void RemoveSmallArea(const vector<vector<int>>& matrix, int iLabel, GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (matrix[i][j] == iLabel||matrix[i][j]==0)//��������������ɫ�������
					continue;
				int offset = i*width + j;
				*(pd + offset) = (BYTE)0;
			}
		}
	}

	//������ܵ�������ͨ����
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
		//ֻ��������İ�ɫ���򣬲��ܴ�������İ�ɫ����
		//���Ͻǿ�ʼ
		for (int i = 0; i < height; i++)
		{
			bool bExit = false;
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//�����λ�Ѿ���ǹ������ô���
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//���һ�����ӵ�
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if (bExit)
				break;
		}
		//���Ͻǿ�ʼ
		for (int i = 0; i < height; i++)
		{
			bool bExit = false;
			for (int j = width-1; j >= 0; j--)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//�����λ�Ѿ���ǹ������ô���
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//���һ�����ӵ�
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if( bExit )
				break;
		}
		//���½ǿ�ʼ
		for (int i = height - 1; i >= 0; i--)
		{
			bool bExit = false;
			for (int j = 0; j < width; j++)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//�����λ�Ѿ���ǹ������ô���
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//���һ�����ӵ�
					LabelGrow(label, j, i, gData, matrix);
					bExit = true;
					break;
				}
			}
			if( bExit )
				break;
		}
		//���½ǿ�ʼ
		for (int i = height - 1; i >= 0; i--)
		{
			bool bExit = false;
			for (int j = width -1 ; j>=0; j--)
			{
				t_ps = (int)(*(ps + i*width + j));
				if (t_ps == 0)
					continue;
				if (matrix[i][j] != 0)//�����λ�Ѿ���ǹ������ô���
				{
					bExit = true;
					break;
				}
				else
				{
					label++;
					//���һ�����ӵ�
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
	//������������
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
