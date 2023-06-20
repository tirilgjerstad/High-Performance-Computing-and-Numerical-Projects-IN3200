#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_graph_from_file1.h"
#include <omp.h>

void create_SNN_graph1 (int N, char **table2D, int ***SNN_table) {

  int **x;

  //Allokerer et 2D array
  x = malloc(N * sizeof(*x));
  for (int i = 0; i < N; i++) {
    x[i] = calloc(N, sizeof(x[0]));
  }

  int i, j, k;
  #ifdef _PARALLEL
  #pragma omp parallel for schedule(dynamic) private(i,j,k)
  #endif
  for (i = 0; i < N; i++){
    for (j = 0; j < N; j++) {
      //Finner alle indekser lik 1 i table2D
      if (table2D[i][j] == 1) {
        for (k = 0; k < N; k++) {
          //Finner indeksene i rad i og rad j som er begge 1
          if(table2D[i][k] == 1 && table2D[j][k] == 1) {
            x[i][j] += 1;
          }
        }
      }
    }
  }
  //Setter SNN_table til å peke på arrayet x
  *SNN_table = x;
}
