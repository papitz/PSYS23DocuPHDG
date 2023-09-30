# Psys23 Belegarbeit DennisGottfried PaulHoeppner

Repo zur Belegarbeit im Fach Parallel Systems im SoSe 2023

## Documentation

Documentation can be viewed [here](https://papitz.github.io/PSYS23DocuPHDG/).
To generate it you need to push to GitHub since it is hosted as a github page.
Any push to any **GitHub** branch will generate the Documentation.

## Build

This project is built with cmake and make.
Run these commands to build it:

```
cmake -S . -B build && cmake --build build
```

## Run

Run the program like this:

```
./HeatMatrix ROWS(int) COLS(int) STARTING_HEAT(float) NUMBER_OF_STEPS(int) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float) PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)
```

Or like this

```
./HeatMatrix FILENAME(string) NUMBER_OF_STEPS(int) HEAT_TRANSFER_CONSTANT(float) CONVERGENCE_LIMIT(float) PARALLEL_FLAG(0 or 1) NUMBER_OF_THREADS(int)

```

All the things written in caps should be replaced with actual values

To use a file just use a csv representation of a matrix.
Example:

```csv
0.0,0.0,0.0,0.0
0.0,0.0,100.0,0.0
0.0,222.0,0.0,0.0
0.0,0.0,0.0,0.0
```

## Testing

To run the tests they have to be compiled first.

```
cd test
cmake . && cmake --build .
```

They can then be run like this:

```
./t_HeatMatrix
```
