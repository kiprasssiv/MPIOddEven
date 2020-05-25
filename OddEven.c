#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>



void merge(int *subA, int *tempA, int left, int mid, int right) {

  int h = left;
  int i = left;
  int j = mid + 1;
  while((h <= mid) && (j <= right)) {
    if(subA[h] <= subA[j]) {
      tempA[i] = subA[h++];
    }
    else {
      tempA[i] = subA[j++];
    }
    i++;
  }

  int k = h;

  if(mid < h){
    mid = right;
    k = j;
  }
  while(k <= mid){
    tempA[i++] = subA[k++];
  }

  for(k = left; k <= right; k++) {
    subA[k] = tempA[k];
  }
}

void mergeSort(int *subA, int *tempA, int left, int right) {
  int mid;
    printf("MS ");
  printArray(subA, 10);

  if(left < right){
    mid = (left + right)/2;

    mergeSort(subA, tempA, left, mid);
    mergeSort(subA, tempA, (mid + 1), right);
    merge(subA, tempA, left, mid, right);
  }

}
void finalMergeSort(int *array, int *secondArray, int left, int right) {
  int i;
  printf("merA ");
  printArray(array, 10);
  for(i = left; i < right; i++){
    if(array[i] > array[i + 1]){//break found
      finalMergeSort(array, secondArray, (i + 1), right);
      merge(array, secondArray, left, i, right);
        printf("merB ");
        printArray(array, 10);
      return;
    }
  }
}

void mpiSort(int worldRank, int subArraySize, int *originalArray, int *sorted){
  // Send each subarray to each process
  int *subArray = malloc(subArraySize * sizeof(int));
  int *tempArray = malloc(subArraySize * sizeof(int));

  MPI_Scatter(originalArray, subArraySize, MPI_INT, subArray,
              subArraySize, MPI_INT, 0, MPI_COMM_WORLD);

  if(worldRank == 0){
    printf("YESH\n");
  }

  mergeSort(subArray, tempArray, 0, (subArraySize - 1));

  MPI_Gather(subArray, subArraySize, MPI_INT, sorted,
             subArraySize, MPI_INT, 0, MPI_COMM_WORLD);
  printf("GATHER\n");
  free(subArray);
  free(tempArray);
}

int isCorrect(int *array, int arraySize){
  int i;
  for(i = 0; i < arraySize - 1; i++){
    if(array[i] > array[i + 1]){
      printf("ERROR, sorted not correctly");
      return 0;
    }
  }
  return 1;
}

int* randomArray(int *array, int size){
  int i;
  array = malloc(size * sizeof(int));
  srand(time(0));

  for(i = 0; i < size; i++){
    array[i] = rand() % size;
  }
  return array;
}

void printArray(int *array, int size){
  int i;
  for(i = 0; i < size; i++){
    printf("%d ", array[i]);
  }
  printf("\n");
}

void doTest(int fullSize, int worldSize){
  float startTime;
  int worldRank;
  int* arrayToSort;
  int size = fullSize / worldSize;
  int *sorted;
  arrayToSort = randomArray(arrayToSort, fullSize);
  printArray(arrayToSort, fullSize);


  // Initialize MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  if(worldRank == 0) {
    sorted = malloc(fullSize * sizeof(int));
  }

  startTime = (float)clock() / CLOCKS_PER_SEC;
  mpiSort(worldRank, size, arrayToSort, sorted);

  printArray(arrayToSort, fullSize);

  // Make the final mergeSort call
  if(worldRank == 0) {
    int *otherArray = malloc(fullSize * sizeof(int));

    finalMergeSort(sorted, otherArray, 0, (fullSize - 1));

    float timeElapsed = (float)clock() / CLOCKS_PER_SEC - startTime;

    char newFilePath[50];
    sprintf(newFilePath,"zans.txt");

    FILE *fptr = fopen(newFilePath, "a");
    if(fptr  == NULL){
      printf("Can't open the file\n");
    }

    isCorrect(sorted, fullSize);
    printArray(sorted, fullSize);
    fprintf(fptr ,"Num of Proccesses: %i | Size %i | Time %f \n", worldSize, fullSize, timeElapsed);

    fclose(fptr);
    free(sorted);
    free(otherArray);
  }

	printf("WorldRank %d\n", worldRank);
  free(arrayToSort);
  MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
  int fullSize = atoi(argv[1]);
  int print = atoi(argv[2]);
  int worldSize;
  //char *fileName;


  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  // fileName = getFileName(worldSize);

  //doTest(10000000, worldSize);
  doTest(10, worldSize);
  //doTest(20000000, worldSize);
  //doTest(30000000, worldSize);
  //doTest(40000000, worldSize);
  //doTest(50000000, worldSize);
  //doTest(60000000, worldSize);

  MPI_Finalize();
}

