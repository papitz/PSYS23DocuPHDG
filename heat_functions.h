#ifndef HEAT_FUNCTIONS_H
#define HEAT_FUNCTIONS_H

// Function declarations
#include <opencv4/opencv2/core/matx.hpp>
float calculateNextTempOfTile(float tile, float up, float left, float right,
                              float down);
float **calculateHeatMatrix(float **heatMatrix, int rows, int cols);
bool zeroOrRim(int value, int dimensions);
void setColorForTemperature(float temperature, cv::Vec3b &pixel);
#endif // HEAT_FUNCTIONS_H
