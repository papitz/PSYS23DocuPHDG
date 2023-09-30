#include "../include/HeatMatrix.hpp"
#include <algorithm>
#include <any>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <stdexcept>
#include <vector>

/**
 * @brief Constructor for HeatMatrix. Initializes a matrix that has number of
 * rows and cols as specified in size
 *
 * @param[in] rows Rows of the matrix
 * @param[in] cols Columns of the matrix
 */
HeatMatrix::HeatMatrix(int rows, int cols)
    : matrix(rows, std::vector<float>(cols, 0.0)), matrixRows(rows),
      matrixCols(cols) {}

/**
 * @brief Constructor for HeatMatrix. Initializes a matrix that has number of
 * rows and cols as specified in size
 *
 * @param[in] rows Rows of the matrix
 * @param[in] cols Columns of the matrix
 */
HeatMatrix::HeatMatrix(std::vector<std::vector<float>> matrix)
    : matrix(matrix), matrixRows(matrix.size()), matrixCols(matrix[0].size()) {}

/**
 * @brief Sets the temperature of the matrix at a specified point
 *
 * @param[in] x X-coordinate where to set targetTemp
 * @param[in] y Y-coordinate where to set targetTemp
 * @param[in] targetTemp Temperature to set the field to
 *
 * @throws std::invalid_argument when the point is not in the matrix
 */
void HeatMatrix::setTempAt(int x, int y, float targetTemp) {
    if (x >= matrixRows) {
        throw std::invalid_argument(
            "X-coordinate where temp should be set not in matrix");
    } else if (y >= matrixCols) {
        throw std::invalid_argument(
            "Y-coordinate where temp should be set not in matrix");
    }

    matrix[x][y] = targetTemp;
}

/**
 * @brief Set the temperature in an area of the matrix
 *
 * @param[in] xStart X-coordinate to start from
 * @param[in] xEnd X-coordinate to end at
 * @param[in] yStart Y-coordinate to start from
 * @param[in] yEnd Y-coordinate to end at
 * @param[in] targetTemp Temperature to set
 */
void HeatMatrix::setTempInArea(int xStart, int xEnd, int yStart, int yEnd,
                               float targetTemp) {
    for (int x = xStart; x <= xEnd; x++) {
        for (int y = yStart; y <= yEnd; y++) {
            setTempAt(x, y, targetTemp);
        }
    }
}

/**
 * @brief Get the temperature of the matrix at a specified point
 *
 * @param[in] x X-coordinate where to set targetTemp
 * @param[in] y Y-coordinate where to set targetTemp
 *
 * @throws std::invalid_argument when the point is not in the matrix
 */
float HeatMatrix::getTempAt(int x, int y) {
    if (x >= matrixRows) {
        throw std::invalid_argument(
            "X-coordinate where temp should be set not in matrix");
    } else if (y >= matrixCols) {
        throw std::invalid_argument(
            "Y-coordinate where temp should be set not in matrix");
    }

    return matrix[x][y];
}

/**
 * @brief Returns the number of rows of the matrix
 *
 * @return Number of rows in the matrix
 */
const int HeatMatrix::getNumberOfRows() const { return matrixRows; }

/**
 * @brief Returns the number of cols of the matrix
 *
 * @return Number of cols in the matrix
 */
const int HeatMatrix::getNumberOfCols() const { return matrixCols; }

/**
 * @brief Get the matrix data
 *
 * @return the vector data of the matrix
 */
std::vector<std::vector<float>> HeatMatrix::getMatrixData() { return matrix; }

/**
 * @brief Print the matrix to the console
 *
 */
void HeatMatrix::printMatrix() const {
    for (const std::vector<float> &row : matrix) {
        for (float value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Get a slice of the matrix as a new heatMatrix
 *
 * @param[in] divider number of processes by which to divide the matrix
 * @param[in] processNumber number of the process that wants the matrix slice
 * @return A slice of the matrix suited for mpi calculation
 */
HeatMatrix HeatMatrix::getSliceOfMatrix(int divider, int processNumber) {
    return HeatMatrix(getRawSliceOfMatrix(divider, processNumber));
}

/**
 * @brief Get a raw data slice of the matrix
 *
 * @param[in] divider number of processes by which to divide the matrix
 * @param[in] processNumber number of the process that wants the matrix slice
 * @return A slice of the matrix suited for mpi calculation
 */
std::vector<std::vector<float>>
HeatMatrix::getRawSliceOfMatrix(int divider, int processNumber) {

    int rowsPerProcess = matrixRows / divider;
    // Either row 0 or the second to last row of the next block
    auto startRow =
        processNumber == 0 ? 0 : (processNumber * rowsPerProcess - 1);
    // Either the last row or the second row of the next block
    auto endRow = divider - 1 == processNumber
                      ? matrixCols - 1
                      : (processNumber + 1) * rowsPerProcess;

    std::vector<std::vector<float>> newMatrixVector;
    for (int i = startRow; i <= endRow; i++) {
        newMatrixVector.push_back(std::vector(matrix[i]));
    }

    return newMatrixVector;
}

/**
 * @brief Take a vector of sliced up heatMatrices and put it back together into
 * one heat matrix
 *
 * @param[in] heatMatrices vector of sliced matrices
 * @return HeatMatrix thats put back together
 */
HeatMatrix
HeatMatrix::collectMatricesAfterMPICalc(std::vector<HeatMatrix> heatMatrices) {
    std::vector<std::vector<std::vector<float>>> heatMatricesVectors;

    // Use std::transform to collect the data from the heatMatrices
    std::transform(
        heatMatrices.begin(), heatMatrices.end(),
        std::back_inserter(heatMatricesVectors),
        [](const HeatMatrix &heatMatrix) { return heatMatrix.matrix; });
    std::vector<std::vector<float>> collectedVectors =
        collectRawMatricesAfterMPICalc(heatMatricesVectors);

    // Create a new Matrix from the vectors
    return HeatMatrix(collectedVectors);
}

/**
 * @brief Function to collect sliced up vectors into one big vector
 *
 * @param[in] heatMatricesVectors collection of vectors
 */
std::vector<std::vector<float>> HeatMatrix::collectRawMatricesAfterMPICalc(
    std::vector<std::vector<std::vector<float>>> heatMatricesVectors) {
    std::vector<std::vector<float>> collectedVectors;
    collectedVectors.push_back(heatMatricesVectors[0][0]);
    for (int i = 0; i < heatMatricesVectors.size(); i++) {
        for (int row = 1; row < heatMatricesVectors[i].size() - 1; row++) {
            collectedVectors.push_back(heatMatricesVectors[i][row]);
        }
    }
    int lastIndex = heatMatricesVectors.size() - 1;
    collectedVectors.push_back(
        heatMatricesVectors[lastIndex]
                           [heatMatricesVectors[lastIndex].size() - 1]);

    return collectedVectors;
}

/**
 * @brief accumulate all temperatures of the matrix
 *
 * @return sum of all temps
 */
float HeatMatrix::accumulateAllTemps() const {
    float totalSum = std::accumulate(
        matrix.begin(), matrix.end(), 0.0,
        [](float sum, const std::vector<float> &row) {
            return sum + std::accumulate(row.begin(), row.end(), 0.0);
        });
    return totalSum;
}

/**
 * @brief Check if the HeatMatrix converged or not
 *
 * @param[in] other HeatMatrix to check conversion agains
 * @param[in] convergenceLimit Max delta the temperature can have to be
 * considered converged
 * @param[in] parallelFlag Flag to set if the convergence check is done in
 * parallel or not
 * @return Boolean if the HeatMatrix converged or not
 */
bool HeatMatrix::checkForConversion(HeatMatrix other, float convergenceLimit,
                                    bool parallelFlag) {
    bool converged = true;

#pragma omp parallel for collapse(2) reduction(&& : converged) if (parallelFlag)
    for (int i = 0; i < matrixRows; ++i) {
        for (int j = 0; j < matrixCols; ++j) {
            float delta = std::abs(matrix[i][j] - other.matrix[i][j]);
            if (delta > convergenceLimit) {
                converged = false;
            }
        }
    }

    return converged;
}

/**
 * @brief Subscript operator implementation for reading elements
 *
 */
const std::vector<float> &HeatMatrix::operator[](int index) const {
    return matrix[index];
}

/**
 * @brief Subscript operator implementation for modifying elements
 *
 */
std::vector<float> &HeatMatrix::operator[](int index) { return matrix[index]; }

/**
 * @brief Custom swap function implementation
 *
 * @param[in,out] other HeatMatrix to swap values with
 */
void HeatMatrix::swap(HeatMatrix &other) {
    if (matrixRows != other.matrixRows || matrixCols != other.matrixCols) {
        throw std::invalid_argument("Swap: Matrices need to be of same size");
    }
    std::swap(matrix, other.matrix);
}

/**
 * @brief Equality operator
 *
 * @param[in] other HeatMatrix to compore to
 * @return If the two matrices are equal or not
 */
bool HeatMatrix::operator==(const HeatMatrix &other) const {
    // Check if the dimensions match
    if (matrixRows != other.matrixRows || matrixCols != other.matrixCols) {
        return false;
    }

    // Compare the vectors
    for (int i = 0; i < matrixRows; ++i) {
        if (matrix[i] != other.matrix[i]) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Operator to be able to let google tests properly output the matrices
 * if something goes wrong
 *
 */
std::ostream &operator<<(std::ostream &os, const HeatMatrix &heatMatrix) {
    os << "HeatMatrix: " << heatMatrix.getNumberOfRows() << "x"
       << heatMatrix.getNumberOfCols() << std::endl;
    for (int i = 0; i < heatMatrix.getNumberOfRows(); ++i) {
        for (int j = 0; j < heatMatrix.getNumberOfCols(); ++j) {
            os << heatMatrix[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}
