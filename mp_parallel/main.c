#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define d 256

void search(char *patterns[], int num_patterns, char *txt, int mod)
{
    int N = strlen(txt);

    for (int k = 0; k < num_patterns; k++)
    {
        char *pat = patterns[k];
        int M = strlen(pat);
        int p = 0, h = 1;

        for (int i = 0; i < M - 1; i++)
            h = (h * d) % mod;

        for (int i = 0; i < M; i++)
        {
            p = (d * p + pat[i]) % mod;
        }

        int count = 0;

#pragma omp parallel for reduction(+ : count)
        for (int i = 0; i <= N - M; i++)
        {
            int t = 0;
            for (int j = 0; j < M; j++)
            {
                t = (d * t + txt[i + j]) % mod;
            }

            if (p == t)
            {
                int found = 1;
                for (int j = 0; j < M; j++)
                {
                    if (txt[i + j] != pat[j])
                    {
                        found = 0;
                        break;
                    }
                }
                count += found;
            }
        }

        printf("Total occurrences of '%s': %d\n", pat, count);
    }
}

char *read_file(char *filepath)
{
    FILE *file;
    long file_size;
    char *buffer;

    file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the file content
    buffer = malloc(file_size + 1);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate the string
    fclose(file);

    return buffer;
}

int main()
{
    // struct timeval start, end;

    double start = omp_get_wtime();
    printf("#################################\n");
    printf("Initiating the sequential algorithm...\n");
    char *filepath = "../scripts/controller_input.txt";
    char *buffer = read_file(filepath);
    char *patterns[] = {"R1L1X", "R1XO", "OYO"};
    int mod = 101;

    // Početak merenja vremena
    search(patterns, 3, buffer, mod);
    double end = omp_get_wtime(); // Kraj merenja vremena

    double elapsed_time = end - start; // razlika u sekundama
    // elapsed_time += (end.tv_usec - start.tv_usec) / 1000000.0; // mikrosekunde u sekunde
    printf("Execution time: %f seconds\n", elapsed_time);

    free(buffer);
    printf("Algorithm completed.\n");
    printf("#################################\n");
    return 0;
}
