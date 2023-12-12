#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

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

        // Read each line into the dynamically allocated array
        if (fgets(text[*num_lines], line_length, file) == NULL)
        {
            break; // End of file
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
void horspool_2d_omp(char **text, int num_lines, char *pattern, int m, FILE *output_file)
{
    int shift[256];
    int i, j, index_in_line;

    for (i = 0; i < 256; i++)
        shift[i] = m;

    for (i = 0; i < m - 1; i++)
        shift[pattern[i]] = m - 1 - i;

#pragma omp parallel for private(i, j, index_in_line) shared(text, num_lines, pattern, m, output_file) schedule(dynamic)
    for (i = 0; i < num_lines; i++)
    {
        size_t line_length = strlen(text[i]) - 1; // Exclude newline character

        for (j = 0; j <= (int)(line_length - m);)
        {
            int k = m - 1;

            while (k >= 0 && pattern[k] == text[i][j + k])
                k--;

            if (k < 0)
            {
                // Pattern found, calculate index in line
                index_in_line = j;

// Use critical section to avoid interleaved output
#pragma omp critical
                fprintf(output_file, "For Horspool Search OMP: Line %d, Index %d: pattern - (%s)\n", i + 1, index_in_line, pattern);
                j += m; // Move to the next potential match
            }
            else
            {
                j += shift[text[i][j + m - 1]];
            }
        }
    }
}

int main()
{
    omp_set_num_threads(4);
    int num_lines;
    char **text = readTextFile(&num_lines);

    if (text == NULL || num_lines == 0)
    {
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
        return 1;
    }
    printf("Horspool OMP Method\n");
    printf("Enter the pattern to search: ");
    scanf("%s", pattern);

    int m = strlen(pattern);

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
        return 1;
    }

    double start_time = omp_get_wtime();

    horspool_2d_omp(text, num_lines, pattern, m, output_file);

    double end_time = omp_get_wtime();

    fclose(output_file);

    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    printf("Total Execution Time Taken For Horspool OMP is: %f seconds\n\n", end_time - start_time);

    return 0;
}
