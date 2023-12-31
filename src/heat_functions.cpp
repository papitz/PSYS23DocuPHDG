#include "../include/heat_functions.hpp"
#include <chrono>
#include <cmath>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace std::chrono;

namespace heatFunctions {

using namespace cv;
using namespace std;

/**
 * @brief calculate the temperature of the tile based on its neighbors
 *
 * @param[in,out] tile value of the tile to calculate
 * @param[in] up neighbor
 * @param[in] left neighbor
 * @param[in] right neighbor
 * @param[in] down neighbor
 * @param[in] heatTransferConstant constant that decides how much heat is
 * transfered
 * @return new temp of the tile
 */
float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down, float heatTransferConstant) {

    float newTile = tile + heatTransferConstant * (right + left - 2 * tile) +
                    heatTransferConstant * (up + down - 2 * tile);

    return newTile;
}

/**
 * @brief Check if a value is on the rim of the matrix
 *
 * @param[in] value location of the value
 * @param[in] dimensions dimension of the array
 * @return boolean that shows if the value is either zero or smaller or on the
 * far side of the array
 */
bool zeroOrRim(int value, int dimensions) {
    return value <= 0 || value >= dimensions - 1;
}

/**
 * @brief calculate the next step of the heatMatrix
 *
 * @param[in,out] heatMatrix heatMatrix that goes in
 * @param[in,out] tmpHeatMatrix second heatMatrix so we can check for
 * Convergence
 * @param[in] rows amount of rows
 * @param[in] cols amount of cols
 * @param[in] heatTransferConstant constant that decides how much heat is
 * transfered
 * @param[in] parallelFlag flag that decides if things are calculated in
 * parallel or not
 * @param[in] convergenceLimit limit at which delta of temp we count the matrix
 * as converged
 * @return if matrix is converged or not
 */
bool calculateHeatMatrix(HeatMatrix &heatMatrix, HeatMatrix &tmpHeatMatrix,
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

                tmpHeatMatrix[x][y] = newTile;
            }
        }
    }

    heatMatrix.swap(tmpHeatMatrix);

    return heatMatrix.checkForConversion(tmpHeatMatrix, convergenceLimit,
                                         parallelFlag);
}

/**
 * @brief calculate a color depending on the temp of a tile
 *
 * @param[in] temperature Temperature to set the corresponding color to
 * @param[out] pixel Pixel that will get set
 */
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
