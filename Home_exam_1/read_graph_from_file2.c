#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_graph_from_file2.h"

int cmpfunc(const void *a, const void *b)
{
    return(*(int*)a - *(int*)b);
}


void read_graph_from_file2(char *filename, int *N, int **row_ptr, int **col_idx) {

  FILE *fileptr;
  int nodes,edges, from, to;

  //Aapner filen med fopen
  fileptr = fopen(filename, "r");
  if (fileptr == NULL) {
    perror("fopen: ");
    exit(EXIT_FAILURE);
  }

  //Skipper de to første linjene, henter antall noder og kanter fra linje nr tre.
  fscanf(fileptr, "%*[^\n]\n"); // first line.
  fscanf(fileptr, "%*[^\n]\n"); // second line.
  fscanf(fileptr, "%*c %*s %d %*s %d %*[^\n]\n", &nodes, &edges);

  *N = nodes;

  //Allokerer plass til row_ptr
  (*row_ptr) = calloc((nodes+1), sizeof(int));
  if (*row_ptr == NULL) {
    printf("Allocation of row_ptr failed\n");
    return;
  }

  //Gaar gjennom kantene en gang for aa sette verdiene til row_ptr
  while(EOF != fscanf(fileptr, "%d %d", &from, &to)) {
    //Finner første antall naboer til hver
    if (from != to && from < nodes && from >= 0 && to < nodes && to >= 0) {
      (*row_ptr)[from+1] += 1;
      (*row_ptr)[to+1] += 1;
    }
  }
  //Oppdaterer row_ptr slik at indeksen stemmer
  for (int i = 1; i < nodes+1; i++) {
    (*row_ptr)[i] += (*row_ptr)[i-1];
  }

  //Leser filen fra toppen igjen
  rewind(fileptr);
  //skipper de første 4 linjene
  for (int i = 0; i < 4; i++) {
    fscanf(fileptr, "%*[^\n]\n");
  }

  //Allokerer plass til col_idx
  (*col_idx) = malloc((2*edges) * sizeof(int));

  //Hjelpearray for aa oppdatere col_idx riktig
  int *rowcopy = malloc((nodes+1) * sizeof(int));
  memcpy(rowcopy, (*row_ptr), (nodes +1) * sizeof(int));

  if (*col_idx == NULL) {
    printf("Allocation of col_idx failed\n");
    return;
  }
  if (rowcopy == NULL) {
    printf("Allocation of rowcopy failed\n");
    return;
  }

  //Gaar gjennom kantene paa nytt for aa sette col_idx
  while(EOF != fscanf(fileptr, "%d %d", &from, &to)) {

    if (from != to && from < nodes && from >= 0 && to < nodes && to >= 0) {
      (*col_idx)[rowcopy[from]] = to;
      (*col_idx)[rowcopy[to]] = from;
      rowcopy[from] += 1;
      rowcopy[to] += 1;
    }
  }

  //Sorterer naboene i col_idx i stigende rekkefølge.
  for (int i = 0; i < nodes; i++) {

    int len = (*row_ptr)[i+1] - (*row_ptr)[i];
    int *naboer = malloc(len*sizeof(int));

    for (int j = 0; j < len; j++) {
      naboer[j] = (*col_idx)[(*row_ptr)[i]+j];
    }

    qsort(naboer, len, sizeof(int), cmpfunc);

    for (int j = 0; j < len; j++) {
      (*col_idx)[(*row_ptr)[i]+j] = naboer[j];
    }
    free(naboer);
  }
  
  free(rowcopy);
  fclose(fileptr);
}
