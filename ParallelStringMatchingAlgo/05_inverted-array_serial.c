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

void invertedArraySearch(char **text, int num_lines, char *pattern, int m, FILE *output_file)
{
    clock_t start_time = clock();
    int **invertedArray = (int **)malloc(256 * sizeof(int *));

    for (int i = 0; i < 256; ++i)
    {
        invertedArray[i] = (int *)malloc(2 * sizeof(int));
        invertedArray[i][0] = -1;
        invertedArray[i][1] = -1;
    }

    // Build inverted array
    for (int i = 0; i < num_lines; ++i)
    {
        for (int j = 0; j < strlen(text[i]); ++j)
        {
            char currentChar = text[i][j];
            invertedArray[currentChar][0] = i; // Line number
            invertedArray[currentChar][1] = j; // Index number
        }
    }
    freopen("output_indices.txt", "w", output_file);
    // Search for the pattern in the inverted array
    int x = 0;
    for (int i = 0; i < m; ++i)
    {
        char currentChar = pattern[i];

        if (invertedArray[currentChar][0] == -1)
        {
            fprintf(output_file, "Character '%c' not found in the text\n", currentChar);
            return;
        }
    }

    // If all characters are found, search for the pattern in the text
    for (int i = 0; i < num_lines; ++i)
    {
        int line_length = strlen(text[i]);

        for (int j = 0; j < line_length - m + 1; ++j)
        {
            int k;
            for (k = 0; k < m; ++k)
            {
                if (text[i][j + k] != pattern[k])
                {
                    break;
                }
            }
            if (k == m)
            {
                x = 1;
                fprintf(output_file, "Pattern '%s' found at Line %d, Index %d\n", pattern, i + 1, j);
            }
        }
    }

    if (x == 0)
    {
        fprintf(output_file, "No pattern found\n");
        printf("No pattern found\n");
    }

    // Free memory allocated for inverted array
    for (int i = 0; i < 256; ++i)
    {
        free(invertedArray[i]);
    }
    free(invertedArray);
    clock_t end_time = clock(); // Record the end time
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    fprintf(output_file, "Execution Time: %f seconds\n", cpu_time_used);
    printf("Total Execution Time Taken For Inverted-Array Serial is: %f seconds\n\n", cpu_time_used);
}

int main()
{
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

    printf("Inverted-Array Serial Method\n");
    printf("Enter the pattern to search: ");
    scanf("%s", pattern);

    int m = strlen(pattern);

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
        return 1;
    }

    // Call inverted array search function and write to the output file
    invertedArraySearch(text, num_lines, pattern, m, output_file);

    fclose(output_file);

    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    return 0;
}
