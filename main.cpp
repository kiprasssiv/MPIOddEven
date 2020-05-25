#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool        char
#define true        1
#define false       0
#define channel1    0
#define channel2    1

#define LEFT_PHASE  0
#define RIGHT_PHASE 1

bool sorted = false;
int num_procs, rank;
int size, *nums;

int mpi_commu_basic(int proc, int* target, int* buffer, int channel)
{
    if (rank >= num_procs || proc >= num_procs || proc < 0)
        return -1;
    MPI_Status status;
    MPI_Sendrecv(target, 1, MPI_INT, proc, channel,
                 buffer, 1, MPI_INT, proc, (channel+1)%2,
                 MPI_COMM_WORLD, &status);
    return 0;
}



void exchage_max(int proc, int* target)
{
    int buffer;
    if (mpi_commu_basic(proc, target, &buffer, channel2))
        return;
    if (buffer > *target) {
        *target = buffer;
        sorted = false;
    }
}

void exchage_min(int proc, int* target)
{
    int buffer;
    if (mpi_commu_basic(proc, target, &buffer, channel1))
        return;
    if (*target > buffer){
        *target = buffer;
        sorted = false;}
}

void _single_phase_sort(int* a, int index, int size)
{
    for (int i = index; i < size - 1; i += 2){
        if (a[i] > a[i + 1]) {
            int temp = a[i];
            a[i] = a[i + 1];
            a[i + 1] = temp;
            sorted = false;
            }
    }
}


int main(int argc, char** argv)
{
	int num_procs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //dydis masyvo
    int num = atoi(argv[1]);
    //sukuriamas array
	int *array_in = (int *)malloc(sizeof(int)*num);
	//sukuriama random array
	for (int i = 0; i < num; i++) {
		array_in[i] = ((int)rand()) % 1000;
	}

    //procesoriam isdalina dydi
    num_procs = num_procs > num ? num : num_procs;
    size = (num + num_procs - 1) / num_procs;

    int count;
    nums = malloc(size * sizeof(int));


    bool single_process = num_procs <= 1 ? true : false;
    int front = size * rank,
    tail = front + size - 1;

    while (!sorted) {
        sorted = true;

        /*** even-phase ***/
        if (!single_process) {
            if (rank % 2 == 0 && tail%2 == 0) {
                exchage_min(rank + 1, &array_in[count - 1]);
                }
            if (rank % 2 != 0 && front%2 != 0) {
                exchage_max(rank - 1, &array_in[0]);
                }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        _single_phase_sort(nums, 0, count);

        /*** odd-phase ***/
        if (!single_process) {
            if (tail%2!=0){
                exchage_min(rank + 1, &array_in[count - 1]);
            }
            if (front%2==0) {
                exchage_max(rank - 1, &nums[0]);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        _single_phase_sort(nums, 1, count);

        if (!single_process) {
            bool tmp = sorted;
            MPI_Allreduce(&tmp, &sorted, 1, MPI_CHAR, MPI_BAND, MPI_COMM_WORLD);
        }
    }
    cout<<"Sorted"<<endl;
    DEBUG("#%d leave sorting-loop(%d)\n", rank, count);


    free(nums);

    MPI_Finalize();
}

//#define is_odd(x)   ((x) & 1)
//#define is_even(x)  (!is_odd(x))
//#define another(x)  ((x + 1) % 2)
/*int mpi_commu(int proc, int* target, int send_sz, int* buffer, int recv_sz, int channel)
{
    if (rank >= num_procs || proc >= num_procs || proc < 0)
        return -1;
    MPI_Status status;
    MPI_Sendrecv(target, send_sz, MPI_INT, proc, channel,
                 buffer, recv_sz, MPI_INT, proc, another(channel),
                 MPI_COMM_WORLD, &status);

    int count;
    MPI_Get_count(&status, MPI_INT, &count);
    return count;
}*/
