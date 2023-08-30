#ifndef HEATMATRIX_H
#define HEATMATRIX_H

#include <vector>

class HeatMatrix {
public:
    HeatMatrix(int size);
    HeatMatrix(HeatMatrix &&) = default;
    HeatMatrix(const HeatMatrix &) = default;
    HeatMatrix &operator=(HeatMatrix &&) = default;
    HeatMatrix &operator=(const HeatMatrix &) = default;
    ~HeatMatrix();
    void printMatrix() const;
    void setTempAt(int x, int y, float targetTemp);

private:
    const int matrixSize;
    std::vector<std::vector<int>> matrix;
};

#endif
