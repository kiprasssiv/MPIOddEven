#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//
void mpiSort(int worldRank, int subArraySize, int *originalArray, int *sorted){
  bool sorted = false;
  int odd = 0;

  while(!=sorted){
        //even time
    if(odd == 0){
            sorted = true;
        for(int i=worldRank*subArraySize+odd;i<worldRank*subArraySize+subArraySize;i+=2){
            if(originalArray[i]>originalArray[i+1]){
                int temp = originalArray[i];
                originalArray[i] = originalArray[i+1];
                originalArray[i+1] = temp;
                sorted = false;
            }
        }
        odd = 1;
    }
        //odd time
    else{
        for(int i=worldRank*subArraySize+odd;i<worldRank*subArraySize+subArraySize;i+=2){
            if(originalArray[i]>originalArray[i+1]){
                int temp = originalArray[i];
                originalArray[i] = originalArray[i+1];
                originalArray[i+1] = temp;
                sorted = false;
            }
        }
        odd = 0;
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

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
