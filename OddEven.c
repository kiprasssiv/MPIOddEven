#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <stdbool.h>

void merge(int *a, int *b, int l, int m, int r) {

    int h, i, j, k;
    h = l;
    i = l;
    j = m + 1;

    while ((h <= m) && (j <= r)) {
        if (a[h] <= a[j]) {
            b[i] = a[h];
            h++;
        } else {
            b[i] = a[j];
            j++;
        }
        i++;
    }

    if (m < h) {
        for (k = j; k <= r; k++) {
            b[i] = a[k];
            i++;
        }
    } else {
        for (k = h; k <= m; k++) {
            b[i] = a[k];
            i++;
        }
    }
    for (k = l; k <= r; k++) {
        a[k] = b[k];
    }
}

void mergeSort(int *a, int *b, int l, int r) {
    int m;
    if (l < r) {
        m = (l + r) / 2;
        mergeSort(a, b, l, m);
        mergeSort(a, b, (m + 1), r);
        merge(a, b, l, m, r);
    }
}

void lastMerge(int *a, int *b, int l, int r, int depth) {
    int m;
    if (l < r && depth > 1) {
        m = (l + r) / 2;
        lastMerge(a, b, l, m, depth / 2);
        lastMerge(a, b, (m + 1), r, depth / 2);
        merge(a, b, l, m, r);
    }
}

int main(int argc, char **argv) {

    int n = 130000000;
    int *origin_arr = NULL;
    clock_t startTime;
    clock_t elapsedTime;

    int c;
    int world_rank;
    int worldSize;

    int *sorted = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    //Masyvo sukurimas ir reiksmiu generavimas
    if (world_rank == 0) {
        origin_arr = malloc(n * sizeof(int));
        srand(1);
        for (c = 0; c < n; c++) {
            int temp = rand();
            origin_arr[c] = temp;
        }

        //Pradedamas laiko skaiciavimas
        startTime = clock();
    }

    //Po kiek isdalinamas masyvas
    int size = n / worldSize;

    //Inicializavimas dalinio masyvo
    int *sub_array = malloc(size * sizeof(int));

    //Isdalinimas po lygiai
    MPI_Scatter(origin_arr, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);

    int *tmp_array = malloc(size * sizeof(int));

    mergeSort(sub_array, tmp_array, 0, (size - 1));

    if (world_rank == 0) {
        sorted = malloc(n * sizeof(int));
    }
    //Masyvu sujungimas
    MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);

    //Galutinis isrikiavimas
    if (world_rank == 0) {
        int *arr = malloc(n * sizeof(int));

        lastMerge(sorted, arr, 0, (n - 1), worldSize);

        //Sustojama skaiciuoti laika
        elapsedTime = clock() - startTime;

        //konvertavimas i sekundes
        double elapsedTime = ((double) elapsedTime) / CLOCKS_PER_SEC; // in seconds

        printf("%f seconds.processors %d /n", elapsedTime, worldSize);
        free(sorted);
        free(arr);
    }
    free(origin_arr);
    free(sub_array);
    free(tmp_array);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

}
