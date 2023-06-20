#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_graph_from_file2.h"
#include <omp.h>

void create_SNN_graph2 (int N, int *row_ptr, int *col_idx, int **SNN_val) {

  //Allokerer plass til array
  int *val = calloc(row_ptr[N], sizeof(int));

  int i,v;
  #ifdef _PARALLEL
  #pragma omp parallel for schedule(dynamic) private(v)
  #endif
  for (i = 0; i < N; i++) {
    for (v = row_ptr[i]; v < row_ptr[i+1]; v++) {
      int j = col_idx[v];  // i og j er naboer

      // i sine naboer
      int *naboer_i = &col_idx[row_ptr[i]];
      int len_i = row_ptr[i+1] - row_ptr[i];

      //j sine naboer
      int *naboer_j = &col_idx[row_ptr[j]];
      int len_j = row_ptr[j+1] - row_ptr[j];

      int ci = 0;
      int cj = 0;

      //Finner felles naboer til i og j ved å sammenligne deres lister med naboer
      while(ci < len_i && cj < len_j) {
        if (*naboer_i == *naboer_j) {
          //printf("yey %d\n", *naboer_i);
          //printf("nabo: %d v: %d \n", *naboer_i, v);
          val[v] += 1;
          //val[j] += 1;
          naboer_i++;
          naboer_j++;
          ci++;
          cj++;
        }

        else if (*naboer_i < *naboer_j) {
          naboer_i++;
          ci++;
        }
        else {
          naboer_j++;
          cj++;
        }
      }
    }
  }
  //Setter SNN_val til å peke på val
  *SNN_val = val;
}
