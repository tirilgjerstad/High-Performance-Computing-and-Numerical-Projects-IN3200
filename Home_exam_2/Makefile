CFLAGS = -g -std=gnu11 -Wall -Wextra
MPIFILES = MPI_main.c MPI_single_layer_convolution.c

main: $(MPIFILES)
	mpicc $(CFLAGS) $(MPIFILES) -o MPI_main

main_ifi: $(MPIFILES)
	/usr/lib64/openmpi/bin/mpicc $(CFLAGS) $(MPIFILES) -o MPI_main

run:
	mpirun -np 4 ./MPI_main 411 400 5

run_ifi:
	/usr/lib64/openmpi/bin/mpirun -np 8 ./MPI_main 718 700 3


clean:
	rm MPI_main
