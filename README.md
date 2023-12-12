# Parallel String Matching Algorithms

This project implements four string matching algorithms in C language: Boyer-Moore, Horspool, Inverted-Array, and Rabin-Karp. Each algorithm is implemented in parallel using OpenMP and MPI, and a serial version is also provided for comparison.

## Algorithms Implemented

1. **Boyer-Moore**
   - Serial Implementation
   - OpenMP Parallel Implementation

2. **Horspool**
   - Serial Implementation
   - OpenMP Parallel Implementation
   - MPI Parallel Implementation

3. **Inverted-Array**
   - Serial Implementation
   - OpenMP Parallel Implementation

4. **Rabin-Karp**
   - Serial Implementation
   - OpenMP Parallel Implementation
   - MPI Parallel Implementation

## Running the Project

To execute the project, follow these steps (in linux terminal):

1. **Clone the repository:**
   -git clone https://github.com/uzairgabol/ParallelStringMatchingAlgo
2. cd ParallelStringMatchingAlgo
3. gcc -o menu menu.c
4. ./menu
5. To change the input file, replace the existing inp_file.txt with your desired input file.

## Results
The project displays the execution time for each algorithm in serial, OpenMP, and MPI modes. These results provide insights into the performance improvements achieved through parallelization.

Feel free to contribute, report issues, or suggest improvements to make this project better. Happy coding! 🚀
