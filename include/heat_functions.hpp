#ifndef HEAT_FUNCTIONS_H
#define HEAT_FUNCTIONS_H

// Function declarations
#include <opencv2/core/matx.hpp>
namespace heatFunctions {

float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down);
bool calculateHeatMatrix(float **heatMatrix, float **tmpHeatMatrix, int rows,
                            int cols);
bool zeroOrRim(int value, int dimensions);
void setColorForTemperature(float temperature, cv::Vec3b &pixel);
} // namespace heatFunctions
#endif // HEAT_FUNCTIONS_ 
