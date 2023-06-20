# README for Home exam 1 IN3200

For å kompliere og kjøre hele programmet sekvensielt:
```
$ make main
$ make run

```   
For å kompliere og kjøre hele programmet med openMP:

```
$ make main_p
$ make run

```
For å nullstille mellom dem:
```
$ make clean

```

Alle filene, main.c read_graph_from_file1.c read_graph_from_file2.c create_SNN_graph1.c create_SNN_graph2.c må kompliseres.
main.c er programmet som kjører alle de andre programmene, og skriver ut resultater.

Programmet kjører med flaggene gcc -std=gnu11 -g -fopenmp

For å skille mellom openMP og sekvensiell legges flagget -D_PARALLEL til for å kjøre med openMP.

Grafene for test-settet med 5 noder skrives ut ingen tid blir tatt for disse, deretter tas tiden for facebook_combined.txt med 4039 noder. Her skrives kun kjøretidene ut for hver av funksjoenene.
