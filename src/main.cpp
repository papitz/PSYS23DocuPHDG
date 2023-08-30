#include "../include/heat_functions.hpp"
#include "../include/HeatMatrix.hpp"
#include <chrono>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <string>
using namespace std::chrono;

using namespace cv;
using namespace std;
using namespace heatFunctions;

using namespace cv;
using namespace std;

// Initialize globally with default values
int rows = 100;
int cols = 100;
int maxNumberOfSteps = 1000;
float startingHeat = 15000.0;
float heatTransferConstant = 0.025;
float convergenceLimit = 0.01;
bool parallelFlag = false;
int numberOfThreads = 20;

/**
 * @brief set all values from the parameter list.
 * For now the order is like this: ROWS(int) COLS(int) NUMBER_OF_STEPS(int)
 * STARTING_HEAT(float) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float)
 * PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)
 *
 *
 * @param int argc Number of arguments
 * @param char*[] argv parameter list
 */
void setValuesFromParams(int argc, char *argv[]) {
    // Return if we dont have all arguments
    if (argc < 9) {
        printf("Not enough arguments provided. Defaults are used\n");
        return;
    }

    rows = stoi(argv[1]);
    cols = stoi(argv[2]);
    maxNumberOfSteps = stoi(argv[3]);
    startingHeat = stof(argv[4]);
    heatTransferConstant = stof(argv[5]);
    convergenceLimit = stof(argv[6]);
    parallelFlag = (bool)stoi(argv[7]);
    numberOfThreads = stoi(argv[8]);

    printf("Successfully set values from command line!\n");
    printf("Rows: %d\n", rows);
    printf("Cols: %d\n", cols);
    printf("maxNumberOfSteps: %d\n", maxNumberOfSteps);
    printf("startingHeat: %f\n", startingHeat);
    printf("heatTransferConstant: %f\n", heatTransferConstant);
    printf("convergenceLimit: %f\n", convergenceLimit);
    printf("parallelFlag: %d\n", parallelFlag);
    printf("numberOfThreads: %d\n", numberOfThreads);
}

void createVideo(float **storedMatrices[], int numberOfSteps) {
    cv::VideoWriter videoWriter;
    videoWriter.open("stored_matrices.avi",
                     cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10,
                     cv::Size(cols, rows));

    for (int i = 0; i < numberOfSteps; i++) {
        cv::Mat matrix(rows, cols, CV_8UC3);
        for (int j = 0; j < rows; ++j) {
            for (int k = 0; k < cols; ++k) {
                cv::Vec3b &pixel = matrix.at<cv::Vec3b>(i, j);
                setColorForTemperature(storedMatrices[i][j][k], pixel);
            }
        }
        videoWriter.write(matrix);
    }

    videoWriter.release();
}

int main(int argc, char *argv[]) {
    setValuesFromParams(argc, argv);
    omp_set_num_threads(numberOfThreads);

    /* float ***storedMatrices = new float **[maxNumberOfSteps]; */

    HeatMatrix heatMatrix = HeatMatrix(rows, cols);
    
    heatMatrix.setTempAt(rows / 2, cols / 2 , startingHeat);

    HeatMatrix tmpHeatMatrix = HeatMatrix(rows, cols);

    int convergedAfterSteps = maxNumberOfSteps;
    bool converged = false;
    
    auto stop = high_resolution_clock::now();
    auto start = high_resolution_clock::now();

    for (int i = 0; i < maxNumberOfSteps; i++) {
        if (i % 25 == 0) {
            printf("Step %d\n", i);
        }

        converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, rows, cols,
                                        heatTransferConstant, parallelFlag,
                                        convergenceLimit);

        stop = high_resolution_clock::now();

        // Store the new matrix in storedMatrices
        /* storedMatrices[i] = newMatrix; */

        if (converged) {
            printf("Converged after %d iterations\n", i);
            convergedAfterSteps = i;
            break;
        }
    }

    auto duration = duration_cast<milliseconds>(stop - start);
    printf("Took %li ms\n", duration.count());
    heatMatrix.printMatrix();

    /* createVideo(storedMatrices, convergedAfterSteps); */

    return 0;
}
