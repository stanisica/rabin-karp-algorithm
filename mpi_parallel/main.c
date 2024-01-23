#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define d 256

int search(char *pat, char *txt, int q)
{
    int M = strlen(pat);
    int N = strlen(txt);
    int i, j;
    int p = 0;
    int t = 0;
    int h = 1;
    int count = 0;

    for (i = 0; i < M - 1; i++)
        h = (h * d) % q;

    for (i = 0; i < M; i++)
    {
        p = (d * p + pat[i]) % q;
        t = (d * t + txt[i]) % q;
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
                count++;
        }

        if (i < N - M)
        {
            t = (d * (t - txt[i] * h) + txt[i + M]) % q;
            if (t < 0)
                t = (t + q);
        }
    }
    return count;
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

int main(int argc, char *argv[])
{
    int size, rank, text_size;
    char *text = NULL;
    int local_counts[3] = {0, 0, 0};
    int global_counts[3] = {0, 0, 0};
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        start_time = MPI_Wtime();
        char *filepath = "../scripts/controller_input.txt";
        text = read_file(filepath);
        if (text == NULL)
        {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        text_size = strlen(text);
    }

    MPI_Bcast(&text_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int segment_size = text_size / size;
    char *segment = malloc(segment_size + 1);

    MPI_Scatter(text, segment_size, MPI_CHAR, segment, segment_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    segment[segment_size] = '\0';

    char *patterns[] = {"R1L1X", "R1XO", "OYO"};
    int num_patterns = 3;

    for (int i = 0; i < num_patterns; i++)
    {
        local_counts[i] = search(patterns[i], segment, 101);
    }

    MPI_Reduce(local_counts, global_counts, num_patterns, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        end_time = MPI_Wtime();
        for (int i = 0; i < num_patterns; i++)
        {
            printf("Total occurrences of '%s': %d\n", patterns[i], global_counts[i]);
        }
        printf("Total execution time: %f seconds\n", end_time - start_time);
    }

    free(segment);
    if (rank == 0)
    {
        free(text);
    }

    MPI_Finalize();
    return 0;
}