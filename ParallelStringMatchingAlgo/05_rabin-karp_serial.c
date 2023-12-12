#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

void rabin_karp_serial(char **text, int num_lines, char *pattern, int m, FILE *output_file)
{
    unsigned long long patternHash = hash(pattern, m);

    for (int i = 0; i < num_lines; i++)
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

                fprintf(output_file, "For Rabin-Karp Search Serial: Line %d, Index %d: pattern - (%s)\n", i + 1, j, pattern);
            }

            if (j < (int)(line_length - m))
            {

                lineHash = recalculateHash(lineHash, text[i][j], text[i][j + m], m);
            }
        }
    }
}

int main()
{
    int num_lines;
    size_t max_line_length;
    char **text = readTextFile(&num_lines, &max_line_length);

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

    printf("Rabin-Karp Serial Method\n");
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

    rabin_karp_serial(text, num_lines, pattern, m, output_file);

    clock_t end_time = clock();

    fclose(output_file);

    free(pattern);
    for (int i = 0; i < num_lines; ++i)
    {
        free(text[i]);
    }
    free(text);

    double execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Total Execution Time Taken For Rabin-Karp Serial is: %f seconds\n\n", execution_time);

    return 0;
}
