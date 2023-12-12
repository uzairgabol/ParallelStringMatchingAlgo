#include <stdio.h>
#include <stdlib.h>


int main()
{
    printf("PDC Project Text Searching Algorithms.\n");
    printf("Group Members:\n");
    printf("Uzair Gabol\t 21K-3285\nMubeen Palh\t 21K-3292\nAsim Khan\t 21K-4685\n\n");

    int choice;

    // Menu-driven program
    printf("Choose an algorithm:\n");
    printf("1. Horspool\n");
    printf("2. Rabin-Karp\n");
    printf("3. Inverted Array\n");
    printf("4. Boyer-Moore\n");

    printf("Enter your choice (1 to 4): ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        system("gcc -o horspool_serial 05_horspool_serial.c");
        system("./horspool_serial");
        system("gcc -o horspool_omp 05_horspool_omp.c -fopenmp");
        system("./horspool_omp");
        system("mpicc -o horspool_mpi 05_horspool_mpi.c");
        system("mpirun -np 4 ./horspool_mpi");
        break;
    case 2:
        system("gcc -o rabin-karp_serial 05_rabin-karp_serial.c");
        system("./05_rabin-karp_serial");
        system("gcc -o rabin-karp_omp 05_rabin-karp_omp.c -fopenmp");
        system("./rabin-karp_omp");
        system("mpicc -o rabin-karp_mpi 05_rabin-karp_mpi.c");
        system("mpirun -np 4 ./rabin-karp_mpi");
        break;
    case 3:
        system("gcc -o inverted-array_serial 05_inverted-array_serial.c");
        system("./inverted-array_serial");
        system("gcc -o inverted-array_omp 05_inverted-array_omp.c -fopenmp");
        system("./inverted-array_omp");
        break;
    case 4:
        system("gcc -o boyer-moore_serial 05_boyer-moore_serial.c");
        system("./boyer-moore_serial");
        system("gcc -o boyer-moore_omp 05_boyer-moore_omp.c -fopenmp");
        system("./boyer-moore_omp");
        break;

    default:
        printf("Invalid choice. Exiting...\n");
        break;
    }

    return 0;
}
