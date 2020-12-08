#pragma once
#include "opencv2/opencv.hpp"

struct DFTEditor {
	cv::Mat* planes = new cv::Mat[2];
	cv::Mat amplitude;
	cv::Mat phase;
	double min, max;

	cv::Mat orginal;
	cv::Mat dftMat;
	float* kernel = nullptr;
	int brushSize = 1;
	float color = 0;
	bool brushGauﬂ = false;
	bool frequencyEdits = false;
	bool mirrorDftEdits = true;

	void openImage(const char* path);

	void calcDft();
	void calcInverseDft();

	void calcGauﬂKernel();
	void drawDft(int x, int y);
	void drawOriginal(int x, int y);
	void draw(int x, int y, cv::Mat m);
	bool inRange(int x, int y, cv::Mat m);
};