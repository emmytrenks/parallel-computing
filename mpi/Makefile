COMPILER=CC

EXECUTABLES=1-hello-mpi

all:
	make $(EXECUTABLES)

1-hello-mpi: 1-hello-mpi.c
	$(COMPILER) -o $@ $^

clean:
	rm -f $(GARBAGE) $(EXECUTABLES)
