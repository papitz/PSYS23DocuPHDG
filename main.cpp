/**
 * A small test application for OpenCV
 */
#include "heat_functions.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down) {
    const float HEAT_TRANSFER_CONST = 0.025;

    float newTile = tile + HEAT_TRANSFER_CONST * (right + left - 2 * tile) +
                    HEAT_TRANSFER_CONST * (up + down - 2 * tile);

    return newTile;
}

bool zeroOrRim(int value, int dimensions) {
    return value <= 0 || value >= dimensions;
}

bool calculateHeatMatrix(float **heatMatrix, float **updatedHeatMatrix,
                         int rows, int cols) {
    bool converged = true;
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            float up = zeroOrRim(y + 1, cols) ? 0.0 : heatMatrix[x][y + 1];
            float left = zeroOrRim(x - 1, rows) ? 0.0 : heatMatrix[x - 1][y];
            float right = zeroOrRim(x + 1, rows) ? 0.0 : heatMatrix[x + 1][y];
            float down = zeroOrRim(y - 1, cols) ? 0.0 : heatMatrix[x][y - 1];
            float oldTile = heatMatrix[x][y];
            float newTile =
                calculateNextTempOfTile(oldTile, up, left, right, down);

            updatedHeatMatrix[x][y] = newTile;

            converged = checkForConversion(converged, newTile, oldTile);
        }
    }
    // Update the heat matrix
    for (int i = 0; i < rows; i++) {
        heatMatrix[i] = new float[cols];
        for (int j = 0; j < cols; j++) {
            heatMatrix[i][j] = updatedHeatMatrix[i][j];
        }
    }
    return converged;
}

bool checkForConversion(bool converged, float newTile, float oldTile) {
    if (converged && (newTile - oldTile < -0.1 || newTile - oldTile > 0.1)) {
        converged = false;
    }
    return converged;
}

void setColorForTemperature(float temperature, cv::Vec3b &pixel) {
    switch ((int)(temperature / 75)) {
    case 0:
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
        break;
    case 1:
        pixel[0] = 255;
        pixel[1] = 102;
        pixel[2] = 0;
        break;
    case 2:
        pixel[0] = 0;
        pixel[1] = 255;
        pixel[2] = 255;
        break;
    case 3:
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 255;
        break;
    default:
        pixel[0] = 255;
        pixel[1] = 255;
        pixel[2] = 255;
    }
}

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
        if (calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols)) {
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
