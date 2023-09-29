#include "../include/HeatMatrix.hpp"
#include "../include/heat_functions.hpp"
#include <cstdio>
#include <chrono>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace heatFunctions;

int main(int argc, char **argv) {
    int matrixRows = 5000;
    int matrixCols = 5000;

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
    originalMatrix.setTempAt(2500, 2500, 15000.0);
    float conversionLimit = 0.1;

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
            /* TODO: Proper data here */
            calculateHeatMatrixInnerFunction(hm, tmpHM, hm.getNumberOfRows(),
                                hm.getNumberOfCols(), 0.025f, 0, 1.0f, 1);
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

            calculateHeatMatrix(firstSlice, tmpMatrix,
                                firstSlice.getNumberOfRows(),
                                firstSlice.getNumberOfCols(), 0.025f, 0, 1.0f);

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
                finalMatrix, conversionLimit, false));

        }
        iterations++;
        MPI_Bcast(&notConverged, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
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
