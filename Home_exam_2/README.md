# README for Home exam 2 IN3200

Programmet kan kjøres og kompileres (på mac) ved bruk av makefilen og følgende kommandoer:  

```
$ make main
$ make run

```   
Dersom makefilen ikke benyttes vil programmet kompliseres og kjøres med følgende kommandoer

```
$ mpicc -g -std=gnu11 -Wall -Wextra MPI_main.c MPI_single_layer_convolution.c -o MPI_main
$ mpirun -np x ./MPI_main M N K

```   

her kjøres programmet med 4 prosesser, M = 411, N = 400, K = 5


For å kompliere og kjøre på ifi-server kan makefilen også brukes med følgende kommandoer:

```
$ make main_ifi
$ make run_ifi

```

her kjøres programmet med 4 prosesser, M = 718, N = 700, K = 3

eller kommandoene 

```
$ /usr/lib64/openmpi/bin/mpicc -g -std=gnu11 -Wall -Wextra MPI_main.c MPI_single_layer_convolution.c -o MPI_main
$ /usr/lib64/openmpi/bin/mpirun -np x ./MPI_main M N K

```   
I begge tilfellene skal x erstattes med antall prosesser, og M N og K er variablene som MPI_main skal ta inn. 

For å nullstille mellom dem:

```
$ make clean

```

Begge filene,  MPI_main.c  og MPI_single_layer_convolution.c må kompliseres.
MPI_main.c er programmet som kjører alle de andre programmene, og skriver ut resultater.

Programmet kjører med flaggene gcc -std=gnu11 -g 

Programmet skriver ut input, output og kernel for M og N mindre enn 15, ellers skrives kun det endelige resultatet fra sammenligningen av den sekvensielle kjøringen og kjøringen med MPI. 

Programmet skal fungere for de fleste valg av x, M, N og K, ved antagelsen om at x og K er betydelig mindre enn M og N. 
Programmet gjør to sjekker før det begynner å kjøre. 
Først for antall argumenter. Jeg antar at programmet brukes riktig, men sjekken er der for å sikre at vi ikke prøver å jobbe med argumenter som ikke er kommet inn. 
Den andre sjekken er om K er har en gyldig verdi med tanke på antall kjerner og M. Dersom K er for stor vil ikke koden kjøre siden vi må passe på at ingen prosesser trenger å sende flere rader enn de i utgangspunktet har fått tildelt. 

