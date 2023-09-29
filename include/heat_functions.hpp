#ifndef HEAT_FUNCTIONS_H
#define HEAT_FUNCTIONS_H

// Function declarations
#include "HeatMatrix.hpp"
#include <opencv2/core/matx.hpp>
namespace heatFunctions {

float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down, float heatTransferConstant);
bool calculateHeatMatrix(float **heatMatrix, float **tmpHeatMatrix, int rows,
                         int cols, float heatTransferConstant,
                         bool parallelFlag, float convergenceLimit);
bool calculateHeatMatrix(HeatMatrix &heatMatrix, HeatMatrix &tmpHeatMatrix,
                         int rows, int cols, float heatTransferConstant,
                         bool parallelFlag, float convergenceLimit);
void calculateHeatMatrixInnerFunction(HeatMatrix &heatMatrix,
                                      HeatMatrix &tmpHeatMatrix, int rows,
                                      int cols, float heatTransferConstant,
                                      bool parallelFlag, float convergenceLimit,
                                      int offset);
bool outOfMatrixBounds(int value, int dimensions);
void setColorForTemperature(float temperature, cv::Vec3b &pixel);
} // namespace heatFunctions
#endif // HEAT_FUNCTIONS_
