#include "../include/HeatMatrix.hpp"
#include "../include/heat_functions.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <sstream>
#include <stdexcept>
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
string filename = "";

// Which frame we will save. If its 10 we save every thenth matrix
int videoFrameModulo = 10;

/**
 * @brief set all values from the parameter list.
 * The order is like this if you use it without a file: ROWS(int) COLS(int)
 * STARTING_HEAT(float) NUMBER_OF_STEPS(int) HEAT_TRANSFER_CONSTANT(float)
 * CONVERGENCE_LIMIT(float) PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)
 * VIDEO_FLAG(0 or 1)
 *
 * The order is like this if you use it WITH a file: FILENAME(file)
 * NUMBER_OF_STEPS(int) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float)
 * PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int) VIDEO_FLAG(0 or 1)
 *
 * @param argc Number of arguments
 * @param argv parameter list
 */
void setValuesFromParams(int argc, char *argv[]) {
    // Return if we dont have all arguments
    if (argc < 5) {
        printf("Not enough arguments provided. Defaults are used\n");
        return;
    } else if (argc == 10) {
        matrixRows = stoi(argv[1]);
        matrixCols = stoi(argv[2]);
        startingHeat = stof(argv[3]);
        maxNumberOfSteps = stoi(argv[4]);
        heatTransferConstant = stof(argv[5]);
        convergenceLimit = stof(argv[6]);
        parallelFlag = (bool)stoi(argv[7]);
        numberOfThreads = stoi(argv[8]);
        createVideoFlag = (bool)stoi(argv[9]);

        printf("Successfully set values from command line!\n");
    } else if (argc == 8) {
        filename = argv[1];
        maxNumberOfSteps = stoi(argv[2]);
        heatTransferConstant = stof(argv[3]);
        convergenceLimit = stof(argv[4]);
        parallelFlag = (bool)stoi(argv[5]);
        numberOfThreads = stoi(argv[6]);
        createVideoFlag = (bool)stoi(argv[7]);
    } else {
        printf("Wrong number of arguments provided. Defaults are used\n");
        return;
    }

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

/**
 * @brief Read a csv file into a 2d float vector
 *
 * @param[in] filename name of the file to read from
 */
vector<vector<float>> readCSV(const string &filename) {
    vector<vector<float>> data; // 2D vector to store the CSV data

    ifstream file(filename); // Open the CSV file

    if (!file.is_open()) {
        cerr << "Error: Unable to open the file " << filename << endl;
        return data; // Return an empty vector in case of an error
    }

    string line;
    float maxValue = 0.0;
    while (getline(file, line)) { // Read each line from the file
        vector<float> row;        // Vector to store the values in a single row
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) { // Split the line into cells
            try {
                float value = stof(cell); // Convert cell to float
                maxValue = max(maxValue, value);
                row.push_back(value);
            } catch (const exception &e) {
                // Handle conversion error if a cell is not a valid float
                cerr << "Warning: Ignoring invalid value in CSV: " << cell
                     << endl;
            }
        }

        data.push_back(row); // Add the row to the 2D vector
    }

    file.close(); // Close the file
    // Save the highest temp as starting heat
    startingHeat = maxValue;

    return data;
}

/**
 * @brief Create a video from a vector of heatMatrix datas
 *
 * @param[in] storedMatrices The vector of matrices
 * @param[in] maxTemp The temp that was set at the beginning
 */
void createVideo(vector<vector<vector<float>>> storedMatrices, float maxTemp) {
    cv::VideoWriter videoWriter;
    videoWriter.open("stored_matrices.mp4",
                     cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 10,
                     cv::Size(matrixCols, matrixRows));

    std::cout << "Size of mat " << storedMatrices.size() << std::endl;
    for (int i = 0; i < storedMatrices.size(); i++) {
        cv::Mat matrix(matrixRows, matrixCols, CV_8UC3);
        for (int j = 0; j < matrixRows; ++j) {
            for (int k = 0; k < matrixCols; ++k) {
                cv::Vec3b &pixel = matrix.at<cv::Vec3b>(j, k);
                setColorForTemperature(storedMatrices[i][j][k], maxTemp, pixel);
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

    // Initialize so we can use it outside the scope
    HeatMatrix heatMatrix = (filename.empty())
                                ? HeatMatrix(matrixRows, matrixCols)
                                : HeatMatrix(readCSV(filename));

    if (filename.empty()) {
        heatMatrix.setTempAt(matrixRows / 2, matrixCols / 2, startingHeat);
    } else {
        matrixRows = heatMatrix.getNumberOfRows();
        matrixCols = heatMatrix.getNumberOfCols();
    }

    HeatMatrix tmpHeatMatrix = HeatMatrix(matrixRows, matrixCols);

    int convergedAfterSteps = maxNumberOfSteps;
    bool converged = false;

    auto stop = high_resolution_clock::now();
    auto start = high_resolution_clock::now();

    for (int i = 0; i < maxNumberOfSteps; i++) {
        if (i % 25 == 0) {
            printf("Step %d\n", i);
        }

        converged = calculateHeatMatrix(heatMatrix, tmpHeatMatrix, matrixRows,
                                        matrixCols, heatTransferConstant,
                                        parallelFlag, convergenceLimit);

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

    if (createVideoFlag) {
        createVideo(storedMatrices, startingHeat);
    }

    return 0;
}
