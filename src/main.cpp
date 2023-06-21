#include "../include/heat_functions.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace heatFunctions;


int main() {
    const int rows = 100;
    const int cols = 100;

    float **heatMatrix = new float *[rows];
    for (int i = 0; i < rows; i++) {
        heatMatrix[i] = new float[cols];
        for (int j = 0; j < cols; j++) {
            heatMatrix[i][j] = 0.0;
        }
    }

    heatMatrix[50][50] = 15000.0;

    float **tmpHeatMatrix = new float *[rows];
    for (int i = 0; i < rows; i++) {
        tmpHeatMatrix[i] = new float[cols];
    }

    for (int i = 0; i < 50000; i++) {
        if (heatFunctions::calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols)) {
            printf("Converged after %d iterations\n", i);
            break;
        }
    }

    // for (int i = 0; i < rows; i++) {
    //     for (int j = 0; j < cols; j++) {
    //         //printf("%.2f ", tmpHeatMatrix[i][j]);
    //     }
    //     printf("\n");
    // }

    cv::Mat matrix(rows, cols, CV_8UC3);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cv::Vec3b &pixel = matrix.at<cv::Vec3b>(i, j);
            setColorForTemperature(heatMatrix[i][j], pixel);
        }
    }

    // Display the matrix
    cv::imshow("Heat Matrix", matrix);
    cv::waitKey();

    // Deallocate the memory for heatMatrix
    for (int i = 0; i < rows; i++) {
        delete[] heatMatrix[i];
    }
    delete[] heatMatrix;

    // Deallocate the memory for updatedHeatMatrix
    for (int i = 0; i < rows; i++) {
        delete[] tmpHeatMatrix[i];
    }
    delete[] tmpHeatMatrix;

    return 0;
}
