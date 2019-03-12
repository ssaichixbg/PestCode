#pragma once
#pragma warning(disable:4996)

#include "ImageData_new.h"
#include "PestCount_new.h"
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
	char *path = argv[1];// = "C:\\Users\\yangm\\Downloads\\Pest-code\\pic\\photo\\wz6.bmp";
	//strcpy(path, *(argv + 1));

	int *width = new int;
	int *height = new int;
	vector<BYTE>R;
	vector<BYTE>G;
	vector<BYTE>B;

	ImageDataHelper::ImageToColorData(path, width, height, R, G, B);
	ColorData colorData(R, G, B, *width, *height);
	GrayData grayData;
	ImageDataHelper::ColorDataToGrayData(colorData, grayData);
	int iCount = PestCountHelper::CalcPestCount(colorData, grayData);
	printf("%d", iCount);
	return 0;
}