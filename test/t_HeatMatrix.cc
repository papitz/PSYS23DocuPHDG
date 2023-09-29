#include "../include/HeatMatrix.hpp"
#include "gtest/gtest.h"

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
    /* ASSERT_THROW(matrix1.checkForConversion(matrix3, 0.01, false),
     * std::invalid_argument); */
}

TEST(HeatMatrixTest, setTempInArea){
    HeatMatrix matrix(4,4);
    matrix.setTempInArea(1, 2, 0, 3, 3.14159);
    ASSERT_FLOAT_EQ(matrix.getTempAt(2, 2), 3.14159);
    ASSERT_FLOAT_EQ(matrix.getTempAt(3, 3), 0.0);
}

TEST(HeatMatrixTest, SliceTest) {

    /* TODO: Streamline this into dicts*/
    /**
     * [[1.0,1.0,1.0,1.0,1.0,1.0,],
     * [1.0,1.0,1.0,1.0,1.0,1.0,],
     * [1.0,1.0,1.0,1.0,1.0,1.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,]]
     **/
    HeatMatrix matrix(6, 6);
    /**
     * [[1.0,1.0,1.0,1.0,1.0,1.0,],
     * [1.0,1.0,1.0,1.0,1.0,1.0,],
     * [1.0,1.0,1.0,1.0,1.0,1.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,]]
     **/
    HeatMatrix matrixSlice1(4, 6);
    /**
     * [[1.0,1.0,1.0,1.0,1.0,1.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,],
     * [2.0,2.0,2.0,2.0,2.0,2.0,]]
     **/
    HeatMatrix matrixSlice2(4, 6);

    matrix.setTempInArea(0, 2, 0, 5, 1.0);
    matrix.setTempInArea(3, 5, 0, 5, 2.0);
    matrixSlice1.setTempInArea(0, 2, 0, 5, 1.0);
    matrixSlice1.setTempInArea(3, 3, 0, 5, 2.0);
    matrixSlice2.setTempInArea(0, 0, 0, 5, 1.0);
    matrixSlice2.setTempInArea(1, 3, 0, 5, 2.0);

    HeatMatrix generatedSlice1 = matrix.getSliceOfMatrix(2, 0);
    HeatMatrix generatedSlice2 = matrix.getSliceOfMatrix(2, 1);

    ASSERT_EQ(matrixSlice1, generatedSlice1);
    ASSERT_EQ(matrixSlice2, generatedSlice2);

    // Test if we can put the matrix back together properly
    HeatMatrix collectedMatrix = HeatMatrix::collectMatricesAfterMPICalc(std::vector({generatedSlice1, generatedSlice2}));
    ASSERT_EQ(matrix, collectedMatrix);
    // Print the matrix for debuggging
    /* for (const std::vector<float> &row : collectedMatrix.getMatrixData()) { */
    /*     for (float value : row) { */
    /*         std::cout << value << " "; */
    /*     } */
    /*     std::cout << std::endl; */
    /* } */
}