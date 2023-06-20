CFLAGS = gcc -std=gnu11 -g -fopenmp
FILES = main.c read_graph_from_file1.c read_graph_from_file2.c create_SNN_graph1.c create_SNN_graph2.c

main: $(FILES)
	$(CFLAGS) $(FILES) -o main

main_p: $(FILES)
	$(CFLAGS) -D_PARALLEL $(FILES) -o main

run:
	./main

clean:
	rm main
