#ifndef HEATMATRIX_H
#define HEATMATRIX_H

#include <vector>
/* #include <algorithm> */

class HeatMatrix {
public:
    HeatMatrix(int rows, int cols);
    HeatMatrix(HeatMatrix &&) = default;
    HeatMatrix(const HeatMatrix &) = default;
    /* HeatMatrix &operator=(HeatMatrix &&) = default; */
    /* HeatMatrix &operator=(const HeatMatrix &) = default; */
    /* ~HeatMatrix(); */
    void printMatrix() const;
    void setTempAt(int x, int y, float targetTemp);
    float accumulateAllTemps() const;

    // Subscript operator for reading elements
    const std::vector<float>& operator[](int index) const;

    // Subscript operator for modifying elements
    std::vector<float>& operator[](int index);

    void swap(HeatMatrix& other);
    bool checkForConversion(HeatMatrix other, float conversionLimit, bool parallelFlag);


private:
    const int matrixRows;
    const int matrixCols;
    std::vector<std::vector<float>> matrix;
};

#endif
