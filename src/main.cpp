#include "../include/heat_functions.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <omp.h>

#define THREAD_NUM 20

using namespace cv;
using namespace std;
using namespace heatFunctions;

using namespace cv;
using namespace std;


int main() {
    const int rows = 100;
    const int cols = 100;
    const int numberOfSteps = 10000;

    omp_set_num_threads(THREAD_NUM);

    float ***storedMatrices = new float **[numberOfSteps];

    float **heatMatrix = new float *[rows];
    for (int i = 0; i < rows; i++) {
        heatMatrix[i] = new float[cols];
        for (int j = 0; j < cols; j++) {
            heatMatrix[i][j] = 0.0;
        }
    }

    heatMatrix[rows/2][cols/2] = 15000.0;

    float **tmpHeatMatrix = new float *[rows];
    for (int i = 0; i < rows; i++) {
        tmpHeatMatrix[i] = new float[cols];
    }

    cv::VideoWriter videoWriter;
    videoWriter.open("stored_matrices.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),10, cv::Size(cols, rows));
    int convergedAfterSteps = numberOfSteps;
    bool converged = false;

    for (int i = 0; i < numberOfSteps; i++) {
        
        converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols);
        // Create a new matrix for this iteration
        float **newMatrix = new float *[rows];
        for (int j = 0; j < rows; j++) {
            newMatrix[j] = new float[cols];
            for (int k = 0; k < cols; k++) {
                newMatrix[j][k] = heatMatrix[j][k];
            }
        }

        // Store the new matrix in storedMatrices
        storedMatrices[i] = newMatrix;

        cv::Mat matrix(rows, cols, CV_8UC3);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                cv::Vec3b &pixel = matrix.at<cv::Vec3b>(i, j);
                setColorForTemperature(heatMatrix[i][j], pixel);
            }
        }

        videoWriter.write(matrix);
        if (converged) {
            printf("Converged after %d iterations\n", i);
            convergedAfterSteps = i;
            break;
        }
    }

    videoWriter.release();

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

    // Deallocate the memory for storedMatrices
    for (int i = 0; i < convergedAfterSteps; i++) {
        for (int j = 0; j < rows; j++) {
            delete[] storedMatrices[i][j];
        }
        delete[] storedMatrices[i];
    }
    delete[] storedMatrices;

    return 0;
}
