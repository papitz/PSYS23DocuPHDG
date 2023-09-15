#include "gtest/gtest.h"
#include "../include/heat_functions.hpp"

namespace heatFunctions {

// Test calculateNextTempOfTile function
TEST(HeatFunctionsTest, CalculateNextTempOfTile) {
    float tile = 10.0;
    float up = 20.0;
    float left = 5.0;
    float right = 15.0;
    float down = 25.0;
    float heatTransferConstant = 0.1;

    float newTile = calculateNextTempOfTile(tile, up, left, right, down, heatTransferConstant);

    // Expected result: newTile = 10.0 + 0.1 * (15.0 + 5.0 - 2 * 10.0) + 0.1 * (20.0 + 25.0 - 2 * 10.0) = 13.0
    ASSERT_FLOAT_EQ(newTile, 13.0);
}

// Test zeroOrRim function
TEST(HeatFunctionsTest, ZeroOrRim) {
    int dimensions = 5;

    // Test values on the rim
    ASSERT_TRUE(zeroOrRim(0, dimensions));
    ASSERT_TRUE(zeroOrRim(4, dimensions));

    // Test values inside the rim
    ASSERT_FALSE(zeroOrRim(1, dimensions));
    ASSERT_FALSE(zeroOrRim(3, dimensions));
}

// Test calculateHeatMatrix function
TEST(HeatFunctionsTest, CalculateHeatMatrix) {
    int rows = 3;
    int cols = 3;
    float heatTransferConstant = 0.1;
    bool parallelFlag = false;
    float convergenceLimit = 0.01;

    HeatMatrix heatMatrix(rows, cols);
    HeatMatrix tmpHeatMatrix(rows, cols);

    // Initialize heatMatrix with some values
    heatMatrix.setTempAt(1, 1, 100.0);
    heatMatrix.setTempAt(0, 0, 0.0);

    bool converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols, heatTransferConstant, parallelFlag, convergenceLimit);

    // Expect that the matrix has not converged yet
    ASSERT_FALSE(converged);

    // Perform more iterations until convergence
    while (!converged) {
        converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols, heatTransferConstant, parallelFlag, convergenceLimit);
    }

    // Expect that the matrix has converged
    ASSERT_TRUE(converged);
}

// Test setColorForTemperature function
TEST(HeatFunctionsTest, SetColorForTemperature) {
    cv::Vec3b pixel;

    // Test temperature ranges and corresponding colors
    setColorForTemperature(30.0, pixel);
    ASSERT_EQ(pixel, cv::Vec3b(0, 0, 0)); // Black

    setColorForTemperature(80.0, pixel);
    ASSERT_EQ(pixel, cv::Vec3b(255, 102, 0)); // Orange

    setColorForTemperature(150.0, pixel);
    ASSERT_EQ(pixel, cv::Vec3b(0, 255, 255)); // Cyan

    setColorForTemperature(200.0, pixel);
    ASSERT_EQ(pixel, cv::Vec3b(0, 0, 255)); // Blue

    setColorForTemperature(250.0, pixel);
    ASSERT_EQ(pixel, cv::Vec3b(255, 255, 255)); // White
}

} // namespace heatFunctions
