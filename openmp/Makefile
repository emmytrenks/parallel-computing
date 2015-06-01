COMPILER=CC

EXECUTABLES=1-hello-omp 2-text-validation

all:
	make $(EXECUTABLES)

1-hello-omp: 1-hello-omp.c
	$(COMPILER) -o $@ $^
2-text-validation: 2-text-validation.c
	$(COMPILER) -o $@ $^

clean:
	rm -f $(GARBAGE) $(EXECUTABLES)

clean-output:
	rm -f *.pbs.{o,e}*
