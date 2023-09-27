#include "../include/HeatMatrix.hpp"
#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char **argv) {
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

    // Only the main process (rank 0) needs to create and initialize the
    // original matrix
    HeatMatrix originalMatrix(10, 10);

    if (rank == 0) {
        // Split the original matrix into slices and send them to other
        // processes
        for (int dest = 1; dest < num_processes; ++dest) {
            // Get the slice of the matrix that corresponds to the destination
            // process
            HeatMatrix slice =
                originalMatrix.getSliceOfMatrix(num_processes, dest);

            // Determine the size of the data to be sent
            int rows = slice.getNumberOfRows();
            int cols = slice.getNumberOfCols();
            int data_size = rows * cols * sizeof(float);

            // Send the size of the data to the destination process (rank
            // 'dest')
            MPI_Send(&data_size, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

            // Send the actual matrix data to the destination process
            MPI_Send(slice.getMatrixData().data(), rows * cols, MPI_FLOAT, dest,
                     0, MPI_COMM_WORLD);
        }
    } else {
        // Receive the size of the incoming data from the main process
        int data_size;
        MPI_Recv(&data_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        // Calculate the number of elements to receive based on the size
        int num_elements = data_size / sizeof(float);

        // Receive the actual matrix data from the main process
        std::vector<float> received_data(num_elements);
        MPI_Recv(received_data.data(), num_elements, MPI_FLOAT, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Create a HeatMatrix object from the received data
        HeatMatrix receivedSlice(received_data);

        // Perform computations on the received slice
        // You can perform your parallel computations on 'receivedSlice' here

        // Send the updated slice back to the main process
        MPI_Send(receivedSlice.getMatrixData().data(), num_elements, MPI_FLOAT,
                 0, 0, MPI_COMM_WORLD);
    }

    // Gather the updated slices back into the original matrix in the main
    // process
    if (rank == 0) {
        std::vector<HeatMatrix> receivedSlices;
        for (int source = 1; source < num_processes; ++source) {
            HeatMatrix receivedSlice;
            // Receive the updated slice from the source process
            // Use MPI_Recv here
            receivedSlices.push_back(receivedSlice);
        }

        // Combine the received slices into the final matrix
        HeatMatrix finalMatrix =
            HeatMatrix::collectMatricesAfterMPICalc(receivedSlices);

        // Now, finalMatrix contains the combined result
    }

    MPI_Finalize();
    return 0;
}
/* #include <mpi.h> */
/* #include <vector> */
/* #include <iostream> */
/* #include "../include/HeatMatrix.hpp" */
/*  */
/* int main(int argc, char *argv[]) { */
/*     MPI_Init(&argc, &argv); */
/*  */
/*     int mpiRank, mpiSize; */
/*     MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank); */
/*     MPI_Comm_size(MPI_COMM_WORLD, &mpiSize); */
/*  */
/*     std::vector<int> numbers = {1, 2, 3, 4, 5, 6}; */
/*  */
/*     std::vector<int> localVector (numbers.size() / mpiSize); */
/*  */
/*     // Distribute the global vector among all processes */
/*     MPI_Scatter(numbers.data(), numbers.size() / mpiSize, MPI_INT,
 * localVector.data(), numbers.size() / mpiSize, MPI_INT, 0, MPI_COMM_WORLD); */
/*  */
/*     int localSum = 0; */
/*     for (int i = 0; i < localVector.size(); i++) { */
/*         localSum += localVector[i]; */
/*     } */
/*  */
/*     int globalSum = 0; */
/*     MPI_Reduce(&localSum, &globalSum, 1, MPI_INT, MPI_SUM, 0,
 * MPI_COMM_WORLD); */
/*  */
/*     if (mpiRank == 0) { */
/*         std::cout << "Global Sum: " << globalSum << std::endl; */
/*     } */
/*  */
/*     MPI_Finalize(); */
/*     return 0; */
/* } */
