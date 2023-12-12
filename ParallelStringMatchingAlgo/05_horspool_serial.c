#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void horspool_2d(char **text, int num_lines, char *pattern, int m, FILE *output_file)
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

        for (j = 0; j <= line_length - m;)
        {
            int k = m - 1;

            while (k >= 0 && pattern[k] == text[i][j + k])
                k--;

            if (k < 0)
            {

                index_in_line = j;
                fprintf(output_file, "For Horspool Search Serial: Line %d, Index %d: pattern - (%s)\n", i + 1, index_in_line, pattern);
                j += m;
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
    printf("Horspool Serial Method\n");
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
    clock_t start_time = clock();

    horspool_2d(text, num_lines, pattern, m, output_file);

    clock_t end_time = clock();

    fclose(output_file);

    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    double execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Total Execution Time Taken For Horspool Serial is: %f seconds\n\n", execution_time);

    return 0;
}
