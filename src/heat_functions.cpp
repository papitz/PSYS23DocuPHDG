#include "../include/heat_functions.hpp"
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

namespace heatFunctions {

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

#pragma omp parallel
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
                float newTile =
                    calculateNextTempOfTile(oldTile, up, left, right, down);

                updatedHeatMatrix[x][y] = newTile;

                /* TODO: This is slow for some reason. Make a better conversion check*/
#pragma omp critical
                { converged = checkForConversion(converged, newTile, oldTile); }
            }
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
} // namespace heatFunctions
