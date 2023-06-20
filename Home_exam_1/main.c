
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_graph_from_file1.h"
#include "read_graph_from_file2.h"
#include "create_SNN_graph1.h"
#include "create_SNN_graph2.h"

#include <omp.h>


void print_graph1(int N, char **table2D){
  printf("2D array:\n");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", table2D[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_graph2(int N, int *row_ptr, int *col_idx) {
  printf("CRS:\n");
  for (int i = 0; i < (N + 1); i++) {
    printf("%d ", row_ptr[i]);
  }
  printf("\n");

  for (int i = 0; i < row_ptr[N]; i++){
    printf("%d ", col_idx[i]);
  }
  printf("\n\n");

}

void print_SNN1(int N, int **SNN_table) {
  printf("SNN_table:\n");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", SNN_table[i][j]);
    }
    printf("\n");
  }
}

void print_SNN2(int len_val, int *SNN_val) {
  printf("\n");
  printf("SNN_val:\n");
  for (int i = 0; i < len_val; i++) {
    printf("%d ", SNN_val[i]);
  }
  printf("\n");
}

int main(void) {
  int N = 0;
  char **table2D;
  int *row_ptr;
  int *col_idx;
  int **SNN_table;
  int *SNN_val;



  //Tester på testdata for å se at alt er korrtkt.
  read_graph_from_file1("test.txt", &N, &table2D);
  printf("N (main): %d\n\n", N);

  print_graph1(N, table2D);

  read_graph_from_file2("test.txt", &N, &row_ptr, &col_idx);
  print_graph2(N, row_ptr, col_idx);

  create_SNN_graph1(N, table2D, &SNN_table);
  create_SNN_graph2(N, row_ptr, col_idx, &SNN_val);

  print_SNN1(N, SNN_table);
  print_SNN2(row_ptr[N], SNN_val);


  //Tester facebook_combined, og tester tid.

  double start1, start2, end1, end2;

  start1 = omp_get_wtime();

  read_graph_from_file1("facebook_combined.txt", &N, &table2D);
  printf("N (main): %d\n\n", N);
  end1 = omp_get_wtime();


  start2 = omp_get_wtime();

  read_graph_from_file2("facebook_combined.txt", &N, &row_ptr, &col_idx);
  end2 = omp_get_wtime();

  printf("Runtime read_graph_from_file1: %lf\nRuntime read_graph_from_file2: %lf \n", end1-start1, end2-start2);


  start1 = omp_get_wtime();
  create_SNN_graph1(N, table2D, &SNN_table);
  end1 = omp_get_wtime();

  start2 = omp_get_wtime();
  create_SNN_graph2(N, row_ptr, col_idx, &SNN_val);
  end2 = omp_get_wtime();

  printf("Runtime create_SNN_graph1: %lf\nRuntime create_SNN_graph2: %lf \n", end1-start1, end2-start2);



  for (int i = 0; i < N; i++) {
    free(table2D[i]);
    free(SNN_table[i]);
  }

  free(table2D);
  free(col_idx);
  free(row_ptr);
  free(SNN_table);
  free(SNN_val);

  return 0;
}
