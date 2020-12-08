#include "DFTEditor.h"
#include <iostream>
#include <qdebug.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace cv;


void swapQuardrants(Mat m) {
    int width = m.cols / 2;
    int height = m.rows / 2;

    Mat q0(m, Rect(0, 0, width, height));
    Mat q1(m, Rect(width, 0, width, height));
    Mat q2(m, Rect(0, height, width, height));
    Mat q3(m, Rect(width, height, width, height));

    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void DFTEditor::calcDft() {
    planes[0] = Mat_<float>(orginal);
    planes[1] = Mat::zeros(orginal.size(), CV_32F);
    merge(planes, 2, dftMat);
    dft(dftMat, dftMat, DFT_COMPLEX_OUTPUT);
    split(dftMat, planes);
    // Right now planes[0] = real and planes[1] = imgaginary
    // In order to recover the arithmetic representation from
    // the polar form we need to calulate magnitude AND phase


    magnitude(planes[0], planes[1], amplitude);  // planes[0] = |F(x,y)|  

    phase = Mat(planes[1].size(), CV_32F);
    for (int i = 0; i < amplitude.rows; i++) {
        for (int j = 0; j < amplitude.cols; j++) {
            phase.at<float>(i, j) = atan2f(planes[0].at<float>(i, j), planes[1].at<float>(i, j));
        }
    }

    amplitude += Scalar::all(1);
    log(amplitude, amplitude); // |F(x,y)| = log(|F(x,y)| + 1)

    minMaxLoc(amplitude, &min, &max);

    normalize(amplitude, amplitude, 0, 1, NORM_MINMAX);
    swapQuardrants(amplitude);

    amplitude.convertTo(dftMat, CV_8UC1, 255);
}

void DFTEditor::calcInverseDft() {
    Mat tempAmp;
    amplitude.copyTo(tempAmp);
    swapQuardrants(tempAmp);
    normalize(tempAmp, tempAmp, min, max, NORM_MINMAX);
    for (int i = 0; i < tempAmp.rows; i++) {
        for (int j = 0; j < tempAmp.cols; j++) {
            float ampli = exp(tempAmp.at<float>(i, j)) - 1;
            planes[0].at<float>(i, j) = ampli * sin(phase.at<float>(i, j));
            planes[1].at<float>(i, j) = ampli * cos(phase.at<float>(i, j));
        }
    }
    Mat tempDft;
    merge(planes, 2, tempDft);
    Mat reconstructed;
    dft(tempDft, reconstructed, DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(reconstructed, reconstructed, 0, 1, NORM_MINMAX);
    reconstructed.convertTo(orginal, CV_8UC1, 255);
}

void DFTEditor::openImage(const char* path) {
    Mat origin = imread(path);
    cvtColor(origin, orginal, COLOR_BGR2GRAY);
    calcDft();
    calcInverseDft(); 
}

void DFTEditor::calcGauﬂKernel() {
    if (kernel != nullptr) {
        delete[] kernel;
    }
    kernel = new float[brushSize * brushSize];
    if (brushGauﬂ) {
        for (int x = 0; x < brushSize; x++) {
            for (int y = 0; y < brushSize; y++) {
                float dx = std::abs(brushSize / 2 - x);
                float dy = std::abs(brushSize / 2 - y);
                float dist = std::sqrtf(dx * dx + dy * dy);
                float c = brushSize / 2;
                kernel[x + brushSize * y] = std::exp(-dist * dist / (c * 10));
            }
        }
    }
    else {
        for (int x = 0; x < brushSize; x++) {
            for (int y = 0; y < brushSize; y++) {
                float dx = std::abs(brushSize / 2 - x);
                float dy = std::abs(brushSize / 2 - y);
                float dist = std::sqrtf(dx * dx + dy * dy);
                if (dist > brushSize / 2) {
                    kernel[x + brushSize * y] = 0;
                }
                else {
                    kernel[x + brushSize * y] = 1;
                }
            }
        }
    }
}

bool DFTEditor::inRange(int x, int y, Mat m) {
    return x >= 0 && y >= 0 && x < m.cols && y < m.rows;
}

void DFTEditor::draw(int x, int y, Mat m) {
    int startX = x - brushSize / 2;
    int startY = y - brushSize / 2;
    int endX = x + brushSize / 2;
    int endY = y + brushSize / 2;
    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            if (inRange(i, j, m)) {
                float alpha = kernel[(i - startX) + brushSize * (j - startY)];
                m.at<float>(j, i) = color * alpha + m.at<float>(j, i) * (1 - alpha);
            }
        }
    }
}

void DFTEditor::drawDft(int x, int y) {
    calcGauﬂKernel();
    if (frequencyEdits) {
        float deltaX = amplitude.cols / 2 - x;
        float deltaY = amplitude.rows / 2 - y;
        float dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        for (float f = 0; f <= 2*M_PI; f+= M_PI/(dist / (brushSize / 2 + 1) * 5)) {
            int dx = std::sin(f) * dist;
            int dy = std::cos(f) * dist;
            draw(amplitude.cols / 2 + dx, amplitude.rows / 2 + dy, amplitude);
        }
    }
    else {
        draw(x, y, amplitude);
        if (mirrorDftEdits) {
            x = amplitude.cols - x;
            y = amplitude.rows - y;
            draw(x, y, amplitude);
        }
    }
    amplitude.convertTo(dftMat, CV_8UC1, 255);
    calcInverseDft();
}

void DFTEditor::drawOriginal(int x, int y) {
    calcGauﬂKernel();
    Mat temp;
    orginal.convertTo(temp, CV_32F, 1./255);
    draw(x, y, temp);
    temp.convertTo(orginal, CV_8UC1, 255);
    calcDft();
}