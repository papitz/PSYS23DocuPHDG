#ifndef HEATMATRIX_H
#define HEATMATRIX_H

#include <vector>

/**
 * @brief class that keeps all data to a heat matrix
 */
class HeatMatrix {
  public:
    HeatMatrix(int rows, int cols);
    HeatMatrix(std::vector<std::vector<float>> matrix);
    HeatMatrix(HeatMatrix &&) = default;
    HeatMatrix(const HeatMatrix &) = default;
    void printMatrix() const;
    void setTempAt(int x, int y, float targetTemp);
    void setTempInArea(int xStart, int xEnd, int yStart, int yEnd,
                       float targetTemp);
    float getTempAt(int x, int y);
    float accumulateAllTemps() const;
    HeatMatrix getSliceOfMatrix(int divider, int processNumber);

    // Subscript operator for reading elements
    const std::vector<float> &operator[](int index) const;

    // Subscript operator for modifying elements
    std::vector<float> &operator[](int index);

    void swap(HeatMatrix &other);
    bool checkForConversion(HeatMatrix other, float conversionLimit,
                            bool parallelFlag);
    int getNumberOfRows();
    int getNumberOfCols();

  private:
    /**
     * @matrixRows Number of rows in the matrix
     */
    const int matrixRows;
    /**
     * @matrixCols Number of columns in the matrix
     */
    const int matrixCols;
    /**
     * @matrix Actual data matrix. Represented as a 2d vector
     */
    std::vector<std::vector<float>> matrix;
};

#endif
