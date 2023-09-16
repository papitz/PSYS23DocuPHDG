#include "gtest/gtest.h"
#include "../include/HeatMatrix.hpp"

// Create a HeatMatrix and check its dimensions
TEST(HeatMatrixTest, ConstructorAndDimensions) {
    HeatMatrix matrix(3, 4);
    ASSERT_EQ(matrix.getNumberOfRows(), 3);
    ASSERT_EQ(matrix.getNumberOfCols(), 4);
    ASSERT_EQ(matrix.getTempAt(1, 2), 0.0f); // Default value
    ASSERT_THROW(matrix.getTempAt(3, 0), std::invalid_argument);
    ASSERT_THROW(matrix.getTempAt(0, 4), std::invalid_argument);
}

// Set and get temperature at a specific point
TEST(HeatMatrixTest, SetAndGetTemp) {
    HeatMatrix matrix(2, 2);
    
    // Set temperature at (0, 1) to 25.5
    matrix.setTempAt(0, 1, 25.5);
    ASSERT_FLOAT_EQ(matrix.getTempAt(0, 1), 25.5);

    // Attempt to set temperature outside matrix bounds
    ASSERT_THROW(matrix.setTempAt(2, 0, 15.0), std::invalid_argument);
    ASSERT_THROW(matrix.setTempAt(0, 2, 15.0), std::invalid_argument);
    ASSERT_THROW(matrix.setTempAt(2, 2, 15.0), std::invalid_argument);
}

// Test matrix accumulation
TEST(HeatMatrixTest, AccumulateAllTemps) {
    HeatMatrix matrix(3, 3);
    // Set some temperatures
    matrix.setTempAt(0, 0, 1.0);
    matrix.setTempAt(1, 2, 2.0);
    matrix.setTempAt(2, 1, 3.0);
    matrix.setTempAt(2, 2, 4.0);
    
    ASSERT_FLOAT_EQ(matrix.accumulateAllTemps(), 10.0); // 1 + 2 + 3 + 4 = 10
}

// Test convergence check
TEST(HeatMatrixTest, CheckForConversion) {
    HeatMatrix matrix1(2, 2);
    HeatMatrix matrix2(2, 2);

    // Set some temperatures in both matrices
    matrix1.setTempAt(0, 0, 1.0);
    matrix1.setTempAt(1, 1, 2.0);
    matrix2.setTempAt(0, 0, 1.0);
    matrix2.setTempAt(1, 1, 2.0);

    // Matrices are equal, so they should converge
    ASSERT_TRUE(matrix1.checkForConversion(matrix2, 0.01, false));

    // Change one value slightly, they should not converge
    matrix2.setTempAt(0, 0, 1.1);
    ASSERT_FALSE(matrix1.checkForConversion(matrix2, 0.01, false));
    
    // Attempt convergence check with different matrix dimensions
    /* HeatMatrix matrix3(3, 3); */
    /* ASSERT_THROW(matrix1.checkForConversion(matrix3, 0.01, false), std::invalid_argument); */
}
