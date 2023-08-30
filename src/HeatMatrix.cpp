#include "../include/HeatMatrix.hpp"
#include <stdexcept>
#include <iostream>

/**
 * @brief Constructor for HeatMatrix. Initializes a matrix that has number of rows and cols as specified in size
 *
 * @param size Number of rows and cols in the matrix
 */
HeatMatrix::HeatMatrix(int size) : matrix(size, std::vector<int>(size, 0)), matrixSize(size){}

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
void HeatMatrix::setTempAt(int x, int y, float targetTemp){
    if (x >= matrixSize) {
        throw std::invalid_argument("X-coordinate where temp should be set not in matrix");
    } else if (y >= matrixSize) {
        throw std::invalid_argument("Y-coordinate where temp should be set not in matrix");
    }

    matrix[x][y] = targetTemp;
}

/**
 * @brief Print the matrix to the console
 *
 */
void HeatMatrix::printMatrix() const {
    for (const std::vector<int>& row : matrix) {
        for (int value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}
