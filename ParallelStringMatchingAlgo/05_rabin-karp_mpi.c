#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define INITIAL_LINE_LENGTH 1000
#define MAX_LINE_LENGTH_INCREMENT 1000
#define PRIME 101

char **readTextFile(int *num_lines, size_t *max_line_length)
{
    FILE *file = fopen("inp_file.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        *num_lines = 0;
        return NULL;
    }

    char **text = NULL;
    *num_lines = 0;
    size_t max_lines = 0;

    while (!feof(file))
    {
        if (*num_lines >= max_lines)
        {
            max_lines += 1;
            text = realloc(text, max_lines * sizeof(char *));
            if (text == NULL)
            {
                perror("Error allocating memory");
                fclose(file);
                *num_lines = 0;
                return NULL;
            }
        }

        size_t line_length = INITIAL_LINE_LENGTH;
        text[*num_lines] = (char *)malloc(line_length * sizeof(char));
        if (text[*num_lines] == NULL)
        {
            perror("Error allocating memory");
            fclose(file);
            for (size_t i = 0; i < *num_lines; ++i)
            {
                free(text[i]);
            }
            free(text);
            *num_lines = 0;
            return NULL;
        }

        if (fgets(text[*num_lines], line_length, file) == NULL)
        {
            break;
        }

        while (strchr(text[*num_lines], '\n') == NULL && !feof(file))
        {
            line_length += MAX_LINE_LENGTH_INCREMENT;
            text[*num_lines] = realloc(text[*num_lines], line_length * sizeof(char));
            if (text[*num_lines] == NULL)
            {
                perror("Error allocating memory");
                fclose(file);
                for (size_t i = 0; i <= *num_lines; ++i)
                {
                    free(text[i]);
                }
                free(text);
                *num_lines = 0;
                return NULL;
            }

            if (fgets(text[*num_lines] + line_length - MAX_LINE_LENGTH_INCREMENT, MAX_LINE_LENGTH_INCREMENT, file) == NULL)
            {
                break;
            }
        }

        if (line_length > *max_line_length)
        {
            *max_line_length = line_length;
        }

        (*num_lines)++;
    }

    fclose(file);
    return text;
}

unsigned long long hash(char *str, int len)
{
    unsigned long long h = 0;
    for (int i = 0; i < len; i++)
    {
        h = (h * PRIME + str[i]) % PRIME;
    }
    return h;
}

unsigned long long recalculateHash(unsigned long long oldHash, char oldChar, char newChar, int len)
{
    unsigned long long newHash = (oldHash - oldChar + PRIME) % PRIME;
    newHash = (newHash * PRIME + newChar) % PRIME;
    return newHash;
}

void rabin_karp_mpi(char **text, int num_lines, char *pattern, int m, FILE *output_file)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned long long patternHash = hash(pattern, m);
    int lines_per_process = num_lines / size;

    for (int i = rank * lines_per_process; i < (rank + 1) * lines_per_process; i++)
    {
        size_t line_length = strlen(text[i]) - 1;

        if (line_length < m)
        {
            continue;
        }

        unsigned long long lineHash = hash(text[i], m);

        for (int j = 0; j <= (int)(line_length - m); j++)
        {
            if (lineHash == patternHash && strncmp(text[i] + j, pattern, m) == 0)
            {
                fprintf(output_file, "For Rabin-Karp Search MPI: Line %d, Index %d: pattern - (%s)\n", i + 1, j, pattern);
            }

            if (j < (int)(line_length - m))
            {
                lineHash = recalculateHash(lineHash, text[i][j], text[i][j + m], m);
            }
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double start_time = MPI_Wtime();

    int num_lines;
    size_t max_line_length;
    char **text = readTextFile(&num_lines, &max_line_length);

    if (text == NULL || num_lines == 0)
    {
        MPI_Finalize();
        return 1;
    }

    char *pattern = (char *)malloc(INITIAL_LINE_LENGTH * sizeof(char));
    if (pattern == NULL)
    {
        perror("Error allocating memory");
        for (int i = 0; i < num_lines; ++i)
        {
            free(text[i]);
        }
        free(text);
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        printf("Rabin-Karp MPI Method\n");
        printf("Enter the pattern to search: ");
        scanf("%s", pattern);
        double start_time = MPI_Wtime();
    }

    int m = strlen(pattern);

    MPI_Bcast(pattern, m, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Open the output file
    FILE *output_file = fopen("output_indices.txt", "w");
    if (output_file == NULL)
    {
        perror("Error creating output file");
        free(pattern);
        for (int i = 0; i < num_lines; ++i)
        {
            free(text[i]);
        }
        free(text);
        MPI_Finalize();
        return 1;
    }

    rabin_karp_mpi(text, num_lines, pattern, m, output_file);

    if (rank == 0)
    {
        double end_time = MPI_Wtime();
        fclose(output_file);
        printf("Total Execution Time Taken For Rabin-Karp MPI is: %f seconds\n\n", end_time - start_time);
    }

    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    MPI_Finalize();
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <mpi.h>

// #define INITIAL_LINE_LENGTH 1000
// #define MAX_LINE_LENGTH_INCREMENT 1000
// #define PRIME 101

// char** readTextFile(int* num_lines, size_t* max_line_length) {
//     FILE *file = fopen("inp_file.txt", "r");
//     if (file == NULL)
//     {
//         perror("Error opening file");
//         *num_lines = 0;
//         return NULL;
//     }

//     // Dynamically allocate memory for the text array
//     char **text = NULL;
//     *num_lines = 0;
//     size_t max_lines = 0;

//     while (!feof(file))
//     {
//         if (*num_lines >= max_lines)
//         {
//             max_lines += 1;
//             text = realloc(text, max_lines * sizeof(char *));
//             if (text == NULL)
//             {
//                 perror("Error allocating memory");
//                 fclose(file);
//                 *num_lines = 0;
//                 return NULL;
//             }
//         }

//         size_t line_length = INITIAL_LINE_LENGTH;
//         text[*num_lines] = (char *)malloc(line_length * sizeof(char));
//         if (text[*num_lines] == NULL)
//         {
//             perror("Error allocating memory");
//             fclose(file);
//             for (size_t i = 0; i < *num_lines; ++i)
//             {
//                 free(text[i]);
//             }
//             free(text);
//             *num_lines = 0;
//             return NULL;
//         }

//         // Read each line into the dynamically allocated array
//         if (fgets(text[*num_lines], line_length, file) == NULL)
//         {
//             break; // End of file
//         }

//         // Check if the line is fully read, if not, expand the buffer
//         while (strchr(text[*num_lines], '\n') == NULL && !feof(file))
//         {
//             line_length += MAX_LINE_LENGTH_INCREMENT;
//             text[*num_lines] = realloc(text[*num_lines], line_length * sizeof(char));
//             if (text[*num_lines] == NULL)
//             {
//                 perror("Error allocating memory");
//                 fclose(file);
//                 for (size_t i = 0; i <= *num_lines; ++i)
//                 {
//                     free(text[i]);
//                 }
//                 free(text);
//                 *num_lines = 0;
//                 return NULL;
//             }

//             // Continue reading the line
//             if (fgets(text[*num_lines] + line_length - MAX_LINE_LENGTH_INCREMENT, MAX_LINE_LENGTH_INCREMENT, file) == NULL)
//             {
//                 break; // End of file
//             }
//         }

//         (*num_lines)++;
//     }

//     fclose(file);
//     return text;
// }

// unsigned long long hash(char* str, int len) {
//     unsigned long long h = 0;
//     for (int i = 0; i < len; i++) {
//         h = (h * PRIME + str[i]) % PRIME;
//     }
//     return h;
// }

// unsigned long long recalculateHash(unsigned long long oldHash, char oldChar, char newChar, int len) {
//     unsigned long long newHash = (oldHash - oldChar + PRIME) % PRIME;
//     newHash = (newHash * PRIME + newChar) % PRIME;
//     return newHash;
// }

// void rabin_karp_mpi(char** text, int num_lines, char* pattern, int m, FILE* output_file) {
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     unsigned long long patternHash = hash(pattern, m);
//     int lines_per_process = num_lines / size;

//     // Rabin-Karp search with MPI parallelization
//     for (int i = rank * lines_per_process; i < (rank + 1) * lines_per_process; i++) {
//         size_t line_length = strlen(text[i]) - 1;  // Exclude newline character

//         if (line_length < m) {
//             continue;  // Skip lines shorter than the pattern
//         }

//         // Initialize hash for the first window in the line
//         unsigned long long lineHash = hash(text[i], m);

//         for (int j = 0; j <= (int)(line_length - m); j++) {
//             if (lineHash == patternHash && strncmp(text[i] + j, pattern, m) == 0) {
//                 // Pattern found, calculate index in line
//                 fprintf(output_file, "For Rabin-Karp Search MPI: Line %d, Index %d: pattern - (%s)\n", i + 1, j, pattern);
//             }

//             if (j < (int)(line_length - m)) {
//                 // Recalculate hash for the next window in the line
//                 lineHash = recalculateHash(lineHash, text[i][j], text[i][j + m], m);
//             }
//         }
//     }

//     MPI_Barrier(MPI_COMM_WORLD);
// }

// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);

//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     int num_lines;
//     size_t max_line_length;
//     char** text = readTextFile(&num_lines, &max_line_length);

//     if (text == NULL || num_lines == 0) {
//         MPI_Finalize();
//         return 1;
//     }

//     char *pattern = (char *)malloc(INITIAL_LINE_LENGTH * sizeof(char));
//     if (pattern == NULL)
//     {
//         perror("Error allocating memory");
//         for (int i = 0; i < num_lines; ++i)
//         {
//             free(text[i]);
//         }
//         free(text);
//         return 1;
//     }

//     printf("Enter the pattern to search: ");
//     scanf("%s", pattern);

//     int m = strlen(pattern);

//     // Open the output file
//     FILE *output_file = fopen("output_indices.txt", "a");
//     if (output_file == NULL)
//     {
//         perror("Error creating output file");
//         free(pattern);
//         for (int i = 0; i < num_lines; ++i)
//         {
//             free(text[i]);
//         }
//         free(text);
//         return 1;
//     }

//     // Call Rabin-Karp search function with MPI parallelization on 2D array and write to the output file
//     rabin_karp_mpi(text, num_lines, pattern, m, output_file);

//     fclose(output_file);

//     // Free dynamically allocated memory
//     free(pattern);
//     for (int i = 0; i < num_lines; ++i)
//     {
//         free(text[i]);
//     }
//     free(text);

//     MPI_Finalize();
//     return 0;
// }
