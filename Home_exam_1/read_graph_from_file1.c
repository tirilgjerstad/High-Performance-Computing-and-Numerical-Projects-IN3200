#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void read_graph_from_file1 (char *filename, int *N, char ***table2D) {

  FILE *fileptr;

  int nodes, edges, fromNode, toNode;

  //Aapner filen
  fileptr = fopen(filename, "r");
  if (fileptr == NULL) {
    perror("fopen: ");
    exit(EXIT_FAILURE);
  }

  //Skipper de to forste linjene, henter ut antall noder og kanter fra linje nr tre.
  fscanf(fileptr, "%*[^\n]\n"); // first line.
  fscanf(fileptr, "%*[^\n]\n"); // second line.
  fscanf(fileptr, "%*c %*s %d %*s %d %*[^\n]\n", &nodes, &edges);

  *N = nodes;

  //Allokerer plass til 2D-arrayet
  char **x;
  x = malloc(nodes * sizeof(*x));
  for (int i = 0; i < nodes; i++) {
    x[i] = calloc(nodes, sizeof(x[0]));
  }

  //Gaar gjennom alle kanter, og setter inn i arrayet som beskrevet.
  while(EOF != fscanf(fileptr, "%d %d", &fromNode, &toNode)) {

    if (fromNode != toNode && fromNode < nodes && fromNode >= 0 && toNode < nodes && toNode >= 0) {

      x[fromNode][toNode] = 1;
      x[toNode][fromNode] = 1;
    }
  }
  //Setter table2D til å peke på x
  *table2D = x;

  fclose(fileptr);
}
