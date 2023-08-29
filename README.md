# Psys23 Belegarbeit DennisGottfried PaulHoeppner

Repo zur Belegarbeit im Fach Parallel Systems im SoSe 2023

## Build

This project is built with cmake and make.
Run these commands to build it:

```
cmake . && make
```

## Run

Run the program like this:

```
./HeatMatrix ROWS(int) COLS(int) NUMBER_OF_STEPS(int) STARTING_HEAT(float) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float) PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)
```

All the things written in caps should be replaced with actual values

## TODO

- [ ] Doxygen
- [ ] Unit Tests
- [ ] MPI
- [ ] Measure Speedup
- [ ] CI/CD Unit Tests
- [ ] Compiler Flags



