#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define INITIAL_LINE_LENGTH 1000
#define MAX_LINE_LENGTH_INCREMENT 1000

char **readTextFile(int *num_lines)
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

        (*num_lines)++;
    }

    fclose(file);
    return text;
}

void horspool_mpi(char **text, int num_lines, char *pattern, int m, FILE *output_file, int rank)
{
    int shift[256];
    int i, j, index_in_line;

    for (i = 0; i < 256; i++)
        shift[i] = m;

    for (i = 0; i < m - 1; i++)
        shift[pattern[i]] = m - 1 - i;

    for (i = 0; i < num_lines; i++)
    {
        size_t line_length = strlen(text[i]) - 1;

        for (j = 0; j <= (int)(line_length - m);)
        {
            int k = m - 1;

            while (k >= 0 && pattern[k] == text[i][j + k])
                k--;

            if (k < 0)
            {

                index_in_line = j;

                if (rank == 0)
                {
                    fprintf(output_file, "For Horspool Search MPI: Line %d, Index %d: pattern - (%s)\n", i + 1, index_in_line, pattern);
                    fflush(output_file);
                }
                j += m;
            }
            else
            {
                j += shift[text[i][j + m - 1]];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    double start_time, end_time;
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int num_lines;
    char **text = readTextFile(&num_lines);

    if (text == NULL || num_lines == 0)
    {
        MPI_Finalize();
        return 1;
    }

    char *pattern = NULL;
    int m;

    if (rank == 0)
    {

        pattern = (char *)malloc(INITIAL_LINE_LENGTH * sizeof(char));
        if (pattern == NULL)
        {
            perror("Error allocating memory");
            MPI_Finalize();
            for (int i = 0; i < num_lines; ++i)
            {
                free(text[i]);
            }
            free(text);
            return 1;
        }

        printf("Horspool MPI Method\n");
        printf("Enter the pattern to search: ");
        scanf("%s", pattern);
        m = strlen(pattern);
        start_time = MPI_Wtime();
    }

    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        pattern = (char *)malloc((m + 1) * sizeof(char));
        if (pattern == NULL)
        {
            perror("Error allocating memory");
            MPI_Finalize();
            for (int i = 0; i < num_lines; ++i)
            {
                free(text[i]);
            }
            free(text);
            return 1;
        }
    }

    MPI_Bcast(pattern, m + 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Open the output file on rank 0
    FILE *output_file = NULL;
    if (rank == 0)
    {
        output_file = fopen("output_indices.txt", "w");
        if (output_file == NULL)
        {
            perror("Error creating output file");
            MPI_Finalize();
            free(pattern);
            for (int i = 0; i < num_lines; ++i)
            {
                free(text[i]);
            }
            free(text);
            return 1;
        }
    }

    horspool_mpi(text, num_lines, pattern, m, output_file, rank);

    if (rank == 0)
    {
        end_time = MPI_Wtime();
        fclose(output_file);
        printf("Total Execution Time Taken For Horspool MPI is: %f seconds\n\n", end_time - start_time);
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
