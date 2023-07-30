#include "../include/heat_functions.hpp"
#include <chrono>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace std::chrono;

namespace heatFunctions {

using namespace cv;
using namespace std;

float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down, float heatTransferConstant) {

    float newTile = tile + heatTransferConstant * (right + left - 2 * tile) +
                    heatTransferConstant * (up + down - 2 * tile);

    return newTile;
}

bool zeroOrRim(int value, int dimensions) {
    return value <= 0 || value >= dimensions;
}

bool calculateHeatMatrix(float **heatMatrix, float **updatedHeatMatrix,
                         int rows, int cols, float heatTransferConstant,
                         bool parallelFlag, float convergenceLimit) {

    /* auto start = high_resolution_clock::now(); */
#pragma omp parallel if (parallelFlag)
    {
#pragma omp for collapse(2)
        for (int x = 0; x < rows; x++) {
            for (int y = 0; y < cols; y++) {
                float up = zeroOrRim(y + 1, cols) ? 0.0 : heatMatrix[x][y + 1];
                float left =
                    zeroOrRim(x - 1, rows) ? 0.0 : heatMatrix[x - 1][y];
                float right =
                    zeroOrRim(x + 1, rows) ? 0.0 : heatMatrix[x + 1][y];
                float down =
                    zeroOrRim(y - 1, cols) ? 0.0 : heatMatrix[x][y - 1];
                float oldTile = heatMatrix[x][y];
                float newTile = calculateNextTempOfTile(
                    oldTile, up, left, right, down, heatTransferConstant);

                updatedHeatMatrix[x][y] = newTile;
            }
        }
    }

    /* TODO: Convergence check with reduction*/
    float** tmpPointer = heatMatrix;
    heatMatrix = updatedHeatMatrix;
    updatedHeatMatrix = tmpPointer;
    /* bool converged = true; */
    /* auto stop = high_resolution_clock::now(); */
    /* auto duration = duration_cast<milliseconds>(stop - start); */
    /* printf("Took %li ms\n", duration.count()); */

    // Update the heat matrix
    /* for (int i = 0; i < rows; i++) { */
    /*     /* heatMatrix[i] = new float[cols]; */
    /*     for (int j = 0; j < cols; j++) { */
    /*         if (converged && (heatMatrix[i][j] - updatedHeatMatrix[i][j] < */
    /*                               -convergenceLimit || */
    /*                           heatMatrix[i][j] - updatedHeatMatrix[i][j] > */
    /*                               convergenceLimit)) { */
    /*             converged = false; */
    /*         } */
    /*         heatMatrix[i][j] = updatedHeatMatrix[i][j]; */
    /*     } */
    /* } */

    /* return converged; */
    return false;
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
} // namespace heatFunctions
