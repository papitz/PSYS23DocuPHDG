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
/* TODO: Check if the exception checking is impacting performance*/
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
/* TODO: Check if the exception checking is impacting performance*/
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
int HeatMatrix::getNumberOfRows() { return matrixRows; }

/**
 * @brief Returns the number of cols of the matrix
 *
 * @return Number of cols in the matrix
 */
int HeatMatrix::getNumberOfCols() { return matrixCols; }

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
 * @brief Get a slice of the matrix
 *
 * @param[in] divider number of processes by which to divide the matrix
 * @param[in] processNumber number of the process that wants the matrix slice
 * @return A slice of the matrix suited for mpi calculation
 */
HeatMatrix HeatMatrix::getSliceOfMatrix(int divider, int processNumber) {

    int rowsPerProcess = matrixRows / divider;
    // Either row 0 or the second to last row of the next block
    auto startRow = std::max(processNumber * rowsPerProcess - 1, 0);
    // Eihter the last row or the second row of the next block
    auto endRow =
        std::min((processNumber + 1) * rowsPerProcess, matrixCols - 1);

    std::vector<std::vector<float>> newMatrix;
    for (int i = startRow; i <= endRow; i++) {
        newMatrix.push_back(std::vector(matrix[i]));
    }

    // Print the matrix for debuggging
    /* for (const std::vector<float> &row : newMatrix) { */
    /*     for (float value : row) { */
    /*         std::cout << value << " "; */
    /*     } */
    /*     std::cout << std::endl; */
    /* } */

    return HeatMatrix(newMatrix);
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
        throw std::invalid_argument("Matrices need to be of same size");
    }
    std::swap(matrix, other.matrix);
}
