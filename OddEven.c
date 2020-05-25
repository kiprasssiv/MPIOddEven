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
//    printf("some: ");
    int m;
    if (l < r) {
        m = (l + r) / 2;
        mergeSort(a, b, l, m);
        mergeSort(a, b, (m + 1), r);
        merge(a, b, l, m, r);
    }
}

void mergeWithDepth(int *a, int *b, int l, int r, int depth) {
    int m;
    if (l < r && depth > 1) {
        m = (l + r) / 2;
        mergeWithDepth(a, b, l, m, depth / 2);
        mergeWithDepth(a, b, (m + 1), r, depth / 2);
        merge(a, b, l, m, r);
    }
}

int main(int argc, char **argv) {

    int n = 130000000;
    int *original_array = NULL;
    clock_t t;

    int c;
    int world_rank;
    int world_size;
    int *sorted = NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_rank == 0) {
        original_array = malloc(n * sizeof(int));
        srand(1);
        for (c = 0; c < n; c++) {
            int temp = rand();
            original_array[c] = temp;
        }
        t = clock();
    }

    int size = n / world_size;
    int *sub_array = malloc(size * sizeof(int));
    MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
    int *tmp_array = malloc(size * sizeof(int));
    mergeSort(sub_array, tmp_array, 0, (size - 1));

    if (world_rank == 0) {
        sorted = malloc(n * sizeof(int));
    }
    MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        int *other_array = malloc(n * sizeof(int));
        printf("World size %d ", world_size);
        mergeWithDepth(sorted, other_array, 0, (n - 1), world_size);

        t = clock() - t;
        double time_taken = ((double) t) / CLOCKS_PER_SEC; // in seconds

        printf("Elapsed time: %f seconds with %d branduoliu \n", time_taken, world_size);
        printf("\n");
        free(sorted);
        free(other_array);
    }
    free(original_array);
    free(sub_array);
    free(tmp_array);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

}
