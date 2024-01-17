#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define d 256

// ulti: R1L1X
// magic spell: R1XY
// heal: YDY

void search(char *patterns[], int num_patterns, char *txt, int mod)
{
    for (int k = 0; k < num_patterns; k++)
    {
        char *pat = patterns[k];
        int M = strlen(pat);
        int N = strlen(txt);
        int i, j;
        int p = 0, t = 0, count = 0, h = 1; // p hash value for pattern, t hash value for txt

        for (i = 0; i < M - 1; i++)
            h = (h * d) % mod;

        for (i = 0; i < M; i++)
        {
            p = (d * p + pat[i]) % mod;
            t = (d * t + txt[i]) % mod;
        }

        for (i = 0; i <= N - M; i++)
        {
            if (p == t)
            {
                for (j = 0; j < M; j++)
                {
                    if (txt[i + j] != pat[j])
                        break;
                }

                if (j == M)
                {
                    count++;
                }
            }

            if (i < N - M)
            {
                t = (d * (t - txt[i] * h) + txt[i + M]) % mod;
                if (t < 0)
                    t = (t + mod);
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
    struct timeval start, end;

    printf("#################################\n");
    printf("Initiating the sequential algorithm...\n");
    char *filepath = "../scripts/controller_input.txt";
    char *buffer = read_file(filepath);
    char *patterns[] = {"R1L1X", "R1XO", "OYO"};
    int mod = 101;

    gettimeofday(&start, NULL); // Početak merenja vremena
    search(patterns, 3, buffer, mod);
    gettimeofday(&end, NULL); // Kraj merenja vremena

    double elapsed_time = (end.tv_sec - start.tv_sec);         // razlika u sekundama
    elapsed_time += (end.tv_usec - start.tv_usec) / 1000000.0; // mikrosekunde u sekunde
    printf("Execution time: %f seconds\n", elapsed_time);

    free(buffer);
    printf("Algorithm completed.\n");
    printf("#################################\n");
    return 0;
}
