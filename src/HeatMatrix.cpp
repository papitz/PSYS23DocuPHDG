#include "../include/HeatMatrix.hpp"
#include <algorithm>
#include <any>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <stdexcept>

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
 * @brief Print the matrix to the console
 *
 */
void HeatMatrix::printMatrix() const {
    for (const std::vector<float> &row : matrix) {
        for (int value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
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
