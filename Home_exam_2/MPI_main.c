#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

#include "MPI_single_layer_convolution.h"

//Function to print out 2D-arrays of floats
void print_2d_lf(float **arr, int x, int y, char* name) {
  printf("%s\n", name);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      printf("%lf ", arr[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int main (int nargs, char **args)
{
  int M=0, N=0, K=0, my_rank, num_procs;

  float **input=NULL, **output=NULL, **kernel=NULL;

  int canrun = 1;

  MPI_Init(&nargs, &args);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if (nargs < 4) {
    if (my_rank == 0) {
      printf("Useage: <filename> <M> <N> <K> \n");
    }
    canrun = 0;
  }

  else if (atoi(args[3]) > atoi(args[1])/num_procs) {
    if (my_rank == 0) {
      printf("K is too large for this M and number of prosesses \n");
    }
    canrun = 0;
  }

  //We dont want to run the program if we dont gave enought arguments or if K does not fit M and number of prosesses.
  if (!canrun) {
    MPI_Finalize();
    return 0;
  }


  //Want to compare runtime for sequantial and parallell running
  clock_t start, end, start_seq, end_seq, tot, tot_seq;



  if (my_rank==0) {
    // read from command line the values of M, N, and K

    start = clock();

    M = atoi(args[1]);
    N = atoi(args[2]);
    K = atoi(args[3]);

    // allocate 2D array ’input’ with M rows and N columns
    input = (float **)malloc(M * sizeof(float*));
    input[0] = (float *)malloc(M * N * sizeof(float));

    for (int i = 1; i < M; i ++) {
      input[i] = &(input[0][N * i]);
    }

    // allocate 2D array ’output’ with M-K+1 rows and N-K+1 columns
    output = (float **)malloc((M-K+1) * sizeof(float*));
    output[0] = (float *)malloc((M-K+1) * (N-K+1) * sizeof(float));

    for (int i = 1; i < M-K+1; i ++) {
      output[i] = &(output[0][(N-K+1) * i]);
    }


    // allocate the convolutional kernel with K rows and K columns
    kernel = (float **)malloc(K * sizeof(float*));
    kernel[0] = (float *)malloc(K * K * sizeof(float));

    for (int i = 1; i < K; i ++) {
        kernel[i] = &(kernel[0][K * i]);
      }

    // fill 2D array ’input’ with some values
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < N; j++) {
        input[i][j] = ((float) rand() / (float)(RAND_MAX));
      }
    }

    // fill kernel with some values
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < K; j++) {
        kernel[i][j] = ((float) rand() / (float)(RAND_MAX));
      }
    }

    //Print if the array not is too big
    if (N < 15 && M < 15) {
      print_2d_lf(kernel, K, K, "Kernel:");
      print_2d_lf(input, M, N, "Input:");
    }
  }

  // process 0 broadcasts values of M, N, K to all the other processes
  MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank > 0) {
    // allocate the convolutional kernel with K rows and K columns
    kernel = (float **)malloc(K * sizeof(float *));
    kernel[0] = (float *)malloc(K * K * sizeof(float));

    for (int i = 1; i < K; i ++) {
        kernel[i] = &(kernel[0][K * i]);
    }
  }

  // process 0 broadcasts the content of kernel to all the other processes
  MPI_Bcast(kernel[0], K*K, MPI_FLOAT, 0, MPI_COMM_WORLD); //usikker på denne

  // parallel computation of a single-layer convolution
  MPI_single_layer_convolution(M, N, input, K, kernel, output);

  if (my_rank==0) {

    // For example, compare the content of array ’output’ with that is
    // produced by the sequential function single_layer_convolution

    end = clock();
    tot = (double)(end - start);



    //Compare with sequential
    start_seq = clock();
    float **output_seq;
    output_seq = (float **)malloc((M-K+1) * sizeof(float*));
    output_seq[0] = (float *)malloc((M-K+1) * (N-K+1) * sizeof(float));

    for (int i = 1; i < M-K+1; i ++) {
      output_seq[i] = &(output_seq[0][(N-K+1) * i]);
    }

    int i,j,ii,jj;
    double temp;
    for (i=0; i <= M-K; i++) {
      for (j=0; j <= N-K; j++) {

        temp = 0.0;

        for (ii=0; ii<K; ii++) {
          for (jj=0; jj<K; jj++) {
            temp += input[i+ii][j+jj]*kernel[ii][jj];
          }
        }
        output_seq[i][j] = temp;
      }
    }
    end_seq = clock();
    tot_seq = (double)(end_seq-start_seq);


    //Print if the array not is too big
    if (N < 15 && M < 15) {
      print_2d_lf(output, M-K+1, N-K+1, "Output(MPI)");
      print_2d_lf(output_seq, M-K+1, N-K+1, "Output(seq)");
    }

    int check_equal = 1;
    float diff = 0;
    for (int i = 0; i < M-K+1; i++) {
      for (int j = 0; j < N-K+1; j++) {
        if (output[i][j] != output_seq[i][j]) {
          check_equal = 0;
          diff += fabsf(output[i][j]-output_seq[i][j]);
        }
      }
    }
    if (check_equal) {
      printf("\033[22;32mOutput(MPI) and output(seq) are equal. MPI succeeded.\n\033[0m");
    } else {
      printf("\033[22;31mNot equal. MPI failed.\n\033[0m");
      printf("Diff: %lf\n", diff);
    }

    //Compare time
    //For small matriceses, the sequantial code is faster, but when the matriceses gets bigger, the parallell code is faster.
    printf("Parallell: %lu ms\n", 1000*tot/CLOCKS_PER_SEC);
    printf("Sequential: %lu  ms\n", 1000*tot_seq/CLOCKS_PER_SEC);



    //Free stuff for prosess 0
    free(input[0]);
    free(input);
    free(output[0]);
    free(output);
    free(output_seq[0]);
    free(output_seq);
    free(kernel[0]);
    free(kernel);
  }

  MPI_Finalize();
  return 0;

}
