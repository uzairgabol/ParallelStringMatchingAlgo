#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

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

    // Dynamically allocate memory for the text array
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

        // Check if the line is fully read, if not, expand the buffer
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

            // Continue reading the line
            if (fgets(text[*num_lines] + line_length - MAX_LINE_LENGTH_INCREMENT, MAX_LINE_LENGTH_INCREMENT, file) == NULL)
            {
                break; // End of file
            }
        }

        (*num_lines)++;
    }

    fclose(file);
    return text;
}
void boyerMooreHorspool(char **text, int num_lines, char *pattern, int m, FILE *output_file)
{
    int shift[256];
    int i, j, index_in_line;

    // Preprocessing for bad character heuristic
    for (i = 0; i < 256; i++)
        shift[i] = m;

    for (i = 0; i < m - 1; i++)
        shift[pattern[i]] = m - 1 - i;

        // Boyer-Moore-Horspool search
#pragma omp parallel for private(i, j, index_in_line) shared(text, num_lines, pattern, m, output_file)
    for (i = 0; i < num_lines; i++)
    {
        size_t line_length = strlen(text[i]) - 1; // Exclude newline character

        for (j = 0; j <= line_length - m;)
        {
            int k = m - 1;

            while (k >= 0 && pattern[k] == text[i][j + k])
                k--;

            if (k < 0)
            {
                // Pattern found, calculate index in line
                index_in_line = j;
                // Use critical section to avoid conflicts in writing to the output file
#pragma omp critical
                {
                    fprintf(output_file, "For Boyer-Moore-Horspool Search: Line %d, Index %d: pattern - (%s)\n", i + 1, index_in_line, pattern);
                }
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

    // Get user input for pattern
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

    printf("Boyer-Moore OMP Method\n");
    printf("Enter the pattern to search: ");
    scanf("%s", pattern);

    int m = strlen(pattern);

    // Open the output file in write mode to erase current data
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

    // Call Boyer-Moore-Horspool search function on 2D array and write to the output file
    boyerMooreHorspool(text, num_lines, pattern, m, output_file);

    double end_time = omp_get_wtime();

    // Close the output file
    fclose(output_file);

    // Free dynamically allocated memory
    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    printf("Total Execution Time Taken For Boyer-Moore OMP is: %f seconds\n\n", end_time - start_time);

    return 0;
}
