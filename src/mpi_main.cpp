#include <mpi.h>
#include <vector>
#include <iostream>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int mpiRank, mpiSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6};

    std::vector<int> localVector (numbers.size() / mpiSize);

    // Distribute the global vector among all processes
    MPI_Scatter(numbers.data(), numbers.size() / mpiSize, MPI_INT, localVector.data(), numbers.size() / mpiSize, MPI_INT, 0, MPI_COMM_WORLD);

    int localSum = 0;
    for (int i = 0; i < localVector.size(); i++) {
        localSum += localVector[i];
    }

    int globalSum = 0;
    MPI_Reduce(&localSum, &globalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (mpiRank == 0) {
        std::cout << "Global Sum: " << globalSum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
