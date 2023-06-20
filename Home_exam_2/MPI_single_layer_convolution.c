#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "MPI_single_layer_convolution.h"


void MPI_single_layer_convolution (int M, int N, float **input,
                                   int K, float **kernel,
                                   float **output)
{
  //The rank of the proesess and total number of prosesses
  int my_rank, num_procs;

  //Send up, send down, recive on top, recive on bottom
  //Used in send og recv, and to calculate local M_out.
  int s_up, s_down, r_top, r_bot;

  //"general" top and bottom, the same value for all prosesses
  //Used to fill num_out_rows for the Gatherv function.
  int top, bottom;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  //For odd K
  if (K%2 == 1) {
    s_up = s_down = r_top = r_bot = top = bottom = (K-1)/2;

  } else {
    //For even K
    s_down = r_top = top = (K/2)-1;
    s_up = r_bot = bottom = (K/2);
  }

  //Rank 0 does not send up, or recive anything on top
  if (my_rank == 0) {
    s_up = r_top =  0;
  }
  //The last prosess does not send down or recive on bottom
  if(my_rank == num_procs-1) {
    s_down = r_bot = 0;
  }

  //number of rows to each prosess
  int *num_rows = malloc(num_procs * sizeof(int));
  //Total numbers of elements to send for each prosess
  int *sendcounts = malloc(num_procs*sizeof(int));
  //Starting point for where to send form to each prosess
  int *Sdispls = malloc(num_procs*sizeof(int));

  //Rows per prosess
  int proc_rows = M/num_procs;
  //Rest if M is not divideble by num_procs
  int rest = M%num_procs;

  Sdispls[0] = 0;

  for (int rank = 0; rank < num_procs-1; rank++) {
      //Last remainder processes gets an extra row.
      num_rows[rank] = proc_rows + ((rank >= (num_procs - rest)) ? 1:0);
      //Sending (number of rows) x N elements to each prosess
      sendcounts[rank] = num_rows[rank]*N;
      //Statring at where the previous prosess ended
      Sdispls[rank+1] = Sdispls[rank] + sendcounts[rank];
  }
  num_rows[num_procs-1] = proc_rows + ((num_procs-1) >= (num_procs - rest) ? 1:0);
  sendcounts[num_procs-1] = num_rows[num_procs-1]*N;


  //Allocate local input
  if (my_rank > 0) {
    input = (float **)malloc((num_rows[my_rank]+r_top+r_bot) * sizeof(float *));
    input[0] = (float *)malloc((num_rows[my_rank]+r_top+r_bot) * N * sizeof(float));

    for (int i = 1; i < (num_rows[my_rank]+r_top+r_bot); i ++) {
        input[i] = &(input[0][N * i]);
      }
  }

  /*Assuming Scatterv does not support overlap. We need to send unike parts og input to each prosess and then send ghost points between the prosesses.
  It would be more efficiant to send overlapped array, but most compilers does not support this. It is more robust to use send and recv to get the ghost points.*/

  //Send a part of input to all the other prosesses
  MPI_Scatterv(input[0],       //sending form proc 0
               sendcounts,     //number of elements to send
               Sdispls,        //where to find the elements to send
               MPI_FLOAT,      //send elements of this  type
               input[r_top],   //send to this local location
               N*num_rows[my_rank], //number of elements to recive
               MPI_FLOAT,           //recive type
               0,                   //root
               MPI_COMM_WORLD);


   //Sending ghost-rows to neighbours

   //We dont want a deadlock,
   //Odd rank: send first, then recive
   //Even rank: recive first, then send
   MPI_Status s;

   //Odd rows
   if (my_rank % 2) {
     //Odd rows can always send up
     MPI_Send(input[r_top], s_up*N, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
     //send down if not the last prosess
     if (my_rank != num_procs-1) {
       MPI_Send(input[r_top+num_rows[my_rank]-s_down], s_down*N, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
     }

     //Rescive
     if (my_rank != 0) {
       //Recieve from the prosess over
       MPI_Recv(input[0], r_top*N, MPI_FLOAT, my_rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
     }
     if (my_rank != num_procs-1) {
       //Recieve from the prosses under
       MPI_Recv(input[r_top+num_rows[my_rank]], r_bot*N, MPI_FLOAT, my_rank+1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
     }
   } else {
     //Even rows
     //Recieve from under
     if (my_rank != num_procs-1) {
       MPI_Recv(input[r_top+num_rows[my_rank]], r_bot*N, MPI_FLOAT, my_rank+1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
     }
     //Rescive from over
     if (my_rank != 0) {
       MPI_Recv(input[0], r_top*N, MPI_FLOAT, my_rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
     }

     //Send down
     if (my_rank != num_procs-1) {
       MPI_Send(input[r_top+num_rows[my_rank]-s_down], s_down*N, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
     }
     //Send up
     if (my_rank != 0) {
       MPI_Send(input[r_top], s_up*N, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
     }
   }

   //Number of rows in local output
   // = Number of rows in local input - K + 1
   int M_out = num_rows[my_rank]+r_top+r_bot-K+1;

   //Allocate local output
   if (my_rank > 0) {
     output = (float **)malloc(M_out * sizeof(float *));
     output[0] = (float *)malloc(M_out * (N-K+1) * sizeof(float));

     for (int i = 1; i < M_out; i ++) {
       output[i] = &(output[0][(N-K+1) * i]);
     }
  }

  /*

  It may not be optimal with 4 loops inside each other, but I can not see any easy fix so we can avoid it in this case.
  There are many operations to be performed, and each point in the input must be multiplied by up to several points in the kernel. We never multiply the same two points several times because the kernel is constantly moving. K is also assumed to be significantly smaller than M and N.

  Im not sure if we are supposed to try to improve this code, I interpret the task as that it is about parallelizing this with MPI, and that the focus should not be on the efficiency of this code snippet. It may be possible, but in that case I think it will be a bigger task then excepted form us in this exam.

  */

  //The given matrix multiplication calculations
  int i,j,ii,jj;
  double temp;
  for (i=0; i <= M_out-1; i++) {
    for (j=0; j <= N-K; j++) {

      temp = 0.0;

        for (ii=0; ii<K; ii++) {
          for (jj=0; jj<K; jj++) {
            temp += input[i+ii][j+jj]*kernel[ii][jj];
          }
        }
        output[i][j] = temp;
      }
    }

  //Parameters for Gatherv
  int *num_out_rows = malloc(num_procs*sizeof(int));
  int *recvcounts = malloc(num_procs*sizeof(int));
  int *Gdispls = malloc(num_procs*sizeof(int));

  Gdispls[0] = 0;

  for (int rank = 0; rank < num_procs-1; rank++) {
    if (rank == 0) {
      num_out_rows[rank] = num_rows[rank]+bottom-K+1;
    }
    else {
      num_out_rows[rank] = num_rows[rank]+top+bottom-K+1;
    }
    recvcounts[rank] = num_out_rows[rank]*(N-K+1);
    Gdispls[rank+1] = Gdispls[rank] + recvcounts[rank];
  }

  num_out_rows[num_procs-1] = num_rows[num_procs-1]+top-K+1;
  recvcounts[num_procs-1] = num_out_rows[num_procs-1]*(N-K+1);


  //Send calculated output back to root
  MPI_Gatherv(output[0],              //send to this location in root
              recvcounts[my_rank],    //number of elements to be sent
              MPI_FLOAT,              //send this type
              output[0],              //send from this local array
              recvcounts,             //array with number of elements to recv
              Gdispls,                //where in output to send to
              MPI_FLOAT,
              0,
              MPI_COMM_WORLD);

  //Free stuff
  free(num_rows);
  free(sendcounts);
  free(Sdispls);
  free(num_out_rows);
  free(recvcounts);
  free(Gdispls);

  if (my_rank != 0) {
    //Free local input, output and kernel
    free(input[0]);
    free(input);
    free(output[0]);
    free(output);
    free(kernel[0]);
    free(kernel);
  }
}
