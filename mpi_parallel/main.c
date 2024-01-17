#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define d 256

void search(char *patterns[], int num_patterns, char *txt, int mod, int rank, int num_procs)
{
    for (int k = 0; k < num_patterns; k++)
    {
        char *pat = patterns[k];
        int M = strlen(pat);
        int N = strlen(txt);
        int i, j;
        int p = 0, t = 0, count = 0, h = 1;

        for (i = 0; i < M - 1; i++)
            h = (h * d) % mod;

        for (i = 0; i < M; i++)
        {
            p = (d * p + pat[i]) % mod;
            t = (d * t + txt[i]) % mod;
        }

        int local_count = 0;

        for (i = rank; i <= N - M; i += num_procs)
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
                    local_count++;
                }
            }

            if (i < N - M)
            {
                t = (d * (t - txt[i] * h) + txt[i + M]) % mod;
                if (t < 0)
                    t = (t + mod);
            }
        }

        MPI_Allreduce(&local_count, &count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        if (rank == 0)
        {
            printf("Total occurrences of '%s': %d\n", pat, count);
        }
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

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(file_size + 1);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    return buffer;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    printf("#################################\n");
    printf("Initiating the parallel algorithm...\n");

    char *filepath = "../scripts/controller_input.txt";
    char *buffer = read_file(filepath);
    char *patterns[] = {"R1L1X", "R1XO", "OYO"};
    int mod = 101;

    clock_t start_time, end_time;
    double elapsed_time;

    if (rank == 0)
    {
        start_time = clock();
    }

    search(patterns, 3, buffer, mod, rank, num_procs);

    if (rank == 0)
    {
        end_time = clock();
        elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Execution time: %f seconds\n", elapsed_time);
        free(buffer);
    }

    printf("Algorithm completed.\n");
    printf("#################################\n");

    MPI_Finalize();

    return 0;
}
