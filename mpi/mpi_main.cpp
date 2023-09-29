#include "../include/HeatMatrix.hpp"
#include "../include/heat_functions.hpp"
#include <cstdio>
#include <chrono>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace heatFunctions;
using namespace std;


// Initialize globally with default values
int matrixRows = 100;
int matrixCols = 100;
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
 *
 *
 * @param argc Number of arguments
 * @param argv parameter list
 */
void setValuesFromParams(int argc, char *argv[]) {
    // Return if we dont have all arguments
    if (argc < 7) {
        printf("Not enough arguments provided. Defaults are used\n");
        return;
    }

    matrixRows = stoi(argv[1]);
    matrixCols = stoi(argv[2]);
    maxNumberOfSteps = stoi(argv[3]);
    startingHeat = stof(argv[4]);
    heatTransferConstant = stof(argv[5]);
    convergenceLimit = stof(argv[6]);

    printf("Successfully set values from command line!\n");
    printf("Rows: %d\n", matrixRows);
    printf("Cols: %d\n", matrixCols);
    printf("maxNumberOfSteps: %d\n", maxNumberOfSteps);
    printf("startingHeat: %f\n", startingHeat);
    printf("heatTransferConstant: %f\n", heatTransferConstant);
    printf("convergenceLimit: %f\n", convergenceLimit);
}

int main(int argc, char **argv) {

    setValuesFromParams(argc, argv);

    MPI_Init(&argc, &argv);

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (num_processes < 2) {
        std::cerr << "This program requires at least 2 MPI processes."
                  << std::endl;
        MPI_Finalize();
        return 1;
    }

    if (num_processes > matrixRows) {
        std::cerr << "This program requires less processes than rows in the matrix"
                  << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Only the main process (rank 0) needs to create and initialize the
    // original matrix
    HeatMatrix originalMatrix(matrixRows, matrixCols);
    HeatMatrix finalMatrix(originalMatrix);
    originalMatrix.setTempAt(matrixRows / 2, matrixCols / 2, startingHeat);

    int iterations = 0;
    bool notConverged = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    while (notConverged) {

        if (rank == 0) {
            // Split the original matrix into slices and send them to other
            // processes
            for (int dest = 1; dest < num_processes; ++dest) {
                /* std::cout << dest << originalMatrix.getSliceOfMatrix(num_processes, dest) << std::endl; */
                std::vector<std::vector<float>> slice =
                    originalMatrix.getRawSliceOfMatrix(num_processes, dest);
                int dataSize = slice.size();
                MPI_Send(&dataSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

                for (const std::vector<float> &data : slice) {
                    int dataSize = data.size();
                    MPI_Send(&dataSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                    MPI_Send(data.data(), dataSize, MPI_FLOAT, dest, 0,
                             MPI_COMM_WORLD);
                }
            }
        } else {
            // Receive and process data in worker processes
            int numRows;
            MPI_Recv(&numRows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            std::vector<std::vector<float>> receivedData;

            for (int j = 0; j < numRows; ++j) {
                int innerDataSize;
                MPI_Recv(&innerDataSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                std::vector<float> innerData(innerDataSize);
                MPI_Recv(innerData.data(), innerDataSize, MPI_FLOAT, 0, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                receivedData.push_back(innerData);
            }

            HeatMatrix hm(receivedData);
            HeatMatrix tmpHM(hm.getNumberOfRows(), hm.getNumberOfCols());
            /* std::cout << "rank: " << rank << " rows: " <<
             * hm.getNumberOfRows() */
            /*           << " cols: " << hm.getNumberOfCols() << "\n"; */
            /* std::cout << "rank: " << rank << " before\n"; */
            calculateHeatMatrixInnerFunction(hm, tmpHM, hm.getNumberOfRows(),
                                hm.getNumberOfCols(), heatTransferConstant, false, convergenceLimit, 1);
            /* std::cout << "rank: " << rank << " after\n"; */
            /* hm.printMatrix(); */

            MPI_Send(&numRows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            receivedData = hm.getMatrixData();

            // Send the processed data back to the master process
            for (const std::vector<float> &data : receivedData) {
                int dataSize = data.size();
                MPI_Send(&dataSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                MPI_Send(data.data(), dataSize, MPI_FLOAT, 0, 0,
                         MPI_COMM_WORLD);
            }
        }

        // Gather the updated slices back into the original matrix in the main
        // process
        if (rank == 0) {
            std::vector<HeatMatrix> receivedSlices;
            // Do the calculation for the first slice
            HeatMatrix firstSlice =
                originalMatrix.getSliceOfMatrix(num_processes, 0);
            HeatMatrix tmpMatrix(firstSlice.getNumberOfRows(),
                                 firstSlice.getNumberOfCols());

            calculateHeatMatrixInnerFunction(firstSlice, tmpMatrix,
                                firstSlice.getNumberOfRows(),
                                firstSlice.getNumberOfCols(), heatTransferConstant, false, convergenceLimit, 0);

            receivedSlices.push_back(firstSlice);

            for (int src = 1; src < num_processes; ++src) {
                std::vector<std::vector<float>> receivedMatrixData;
                int numRows;
                MPI_Recv(&numRows, 1, MPI_INT, src, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
                for (size_t i = 0; i < numRows; ++i) {
                    int numCols;
                    MPI_Recv(&numCols, 1, MPI_INT, src, 0, MPI_COMM_WORLD,
                             MPI_STATUS_IGNORE);

                    std::vector<float> receivedData(numCols);
                    MPI_Recv(receivedData.data(), numCols, MPI_FLOAT, src, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Process and print the received data
                    /* std::cout << "Received data from process " << src */
                    /*           << " (Vector " << i << "): "; */
                    /* for (float value : receivedData) { */
                    /*     std::cout << value << " "; */
                    /* } */
                    /* std::cout << std::endl; */
                    receivedMatrixData.push_back(receivedData);
                }
                receivedSlices.push_back(HeatMatrix(receivedMatrixData));
            }
            // Combine the received slices into the final matrix
            HeatMatrix finalMatrix =
                HeatMatrix::collectMatricesAfterMPICalc(receivedSlices);
            /* std::cout << "originalMatrix: \n" << originalMatrix; */
            /* std::cout << "finalMatrix: \n" << finalMatrix; */
            finalMatrix.swap(originalMatrix);

            notConverged = (!originalMatrix.checkForConversion(
                finalMatrix, convergenceLimit, false));

        }
        iterations++;

        MPI_Bcast(&notConverged, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // Stop if we are over the max number of steps
        if (iterations > maxNumberOfSteps) {
            break;
        }
    }

    stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    printf("Took %li ms\n", duration.count());

    if (rank == 0) {
        std::cout << "final after " << iterations << " iterations\n";
        /* originalMatrix.printMatrix(); */
    }

    MPI_Finalize();
    return 0;
}
