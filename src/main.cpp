#include "../include/heat_functions.hpp"
#include "../include/HeatMatrix.hpp"
#include <chrono>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std::chrono;

using namespace cv;
using namespace std;
using namespace heatFunctions;

// Initialize globally with default values
int matrixRows = 100;
int matrixCols = 100;
int maxNumberOfSteps = 1000;
float startingHeat = 15000.0;
float heatTransferConstant = 0.025;
float convergenceLimit = 0.01;
bool parallelFlag = false;
int numberOfThreads = 20;
bool createVideoFlag = false;

// Which frame we will save. If its 10 we save every thenth matrix
int videoFrameModulo = 10;

/**
 * @brief set all values from the parameter list.
 * For now the order is like this: ROWS(int) COLS(int) NUMBER_OF_STEPS(int)
 * STARTING_HEAT(float) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float)
 * PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)
 *
 *
 * @param argc Number of arguments
 * @param argv parameter list
 */
void setValuesFromParams(int argc, char *argv[]) {
    // Return if we dont have all arguments
    if (argc < 9) {
        printf("Not enough arguments provided. Defaults are used\n");
        return;
    }

    matrixRows = stoi(argv[1]);
    matrixCols = stoi(argv[2]);
    maxNumberOfSteps = stoi(argv[3]);
    startingHeat = stof(argv[4]);
    heatTransferConstant = stof(argv[5]);
    convergenceLimit = stof(argv[6]);
    parallelFlag = (bool)stoi(argv[7]);
    numberOfThreads = stoi(argv[8]);
    createVideoFlag = (bool)stoi(argv[9]);


    printf("Successfully set values from command line!\n");
    printf("Rows: %d\n", matrixRows);
    printf("Cols: %d\n", matrixCols);
    printf("maxNumberOfSteps: %d\n", maxNumberOfSteps);
    printf("startingHeat: %f\n", startingHeat);
    printf("heatTransferConstant: %f\n", heatTransferConstant);
    printf("convergenceLimit: %f\n", convergenceLimit);
    printf("parallelFlag: %d\n", parallelFlag);
    printf("numberOfThreads: %d\n", numberOfThreads);
    printf("createVideoFlag: %d\n", createVideoFlag);
}

void createVideo(vector<vector<vector<float>>> storedMatrices, float maxTemp) {
    cv::VideoWriter videoWriter;
    videoWriter.open("stored_matrices_new.avi",
                     cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10,
                     cv::Size(matrixCols, matrixRows));

    std::cout << "Size of mat " << storedMatrices.size() << std::endl;
    for (int i = 0; i < storedMatrices.size(); i++) {
        cv::Mat matrix(matrixRows, matrixCols, CV_8UC3);
        for (int j = 0; j < matrixRows; ++j) {
            for (int k = 0; k < matrixCols; ++k) {
                cv::Vec3b &pixel = matrix.at<cv::Vec3b>(j, k);
                setColorForTemperature(storedMatrices[i][j][k], maxTemp,  pixel);
            }
        }
        videoWriter.write(matrix);
    }

    videoWriter.release();
}

/**
 * @brief Main function for the program
 *
 * @param[in] argc number of parameters
 * @param[in] argv parameters
 * @return wether or not the program was successful
 */
int main(int argc, char *argv[]) {
    setValuesFromParams(argc, argv);
    omp_set_num_threads(numberOfThreads);

    vector<vector<vector<float>>> storedMatrices;

    HeatMatrix heatMatrix = HeatMatrix(matrixRows, matrixCols);
    
    heatMatrix.setTempAt(matrixRows / 2, matrixCols / 2 , startingHeat);

    HeatMatrix tmpHeatMatrix = HeatMatrix(matrixRows, matrixCols);

    int convergedAfterSteps = maxNumberOfSteps;
    bool converged = false;
    
    auto stop = high_resolution_clock::now();
    auto start = high_resolution_clock::now();

    for (int i = 0; i < maxNumberOfSteps; i++) {
        if (i % 25 == 0) {
            printf("Step %d\n", i);
        }

        converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, matrixRows, matrixCols,
                                        heatTransferConstant, parallelFlag,
                                        convergenceLimit);

        if (createVideoFlag && i % videoFrameModulo == 0) {
            storedMatrices.push_back(heatMatrix.getMatrixData());
        }

        if (converged) {
            printf("Converged after %d iterations\n", i);
            convergedAfterSteps = i;
            break;
        }
    }
    stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    printf("Took %li ms\n", duration.count());
    /* heatMatrix.printMatrix(); */

    createVideo(storedMatrices, startingHeat);

    return 0;
}
