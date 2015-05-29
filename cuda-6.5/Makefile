EXECUTABLES=1-calc-pi-cuda

all:
	make $(EXECUTABLES)

1-calc-pi-cuda.o: 1-calc-pi-cuda.cu
	nvcc  -o $@ -c $^
GARBAGE+=1-calc-pi-cuda.o

1-calc-pi-cuda: 1-calc-pi-cuda.o 1-calc-pi.c
	CC -o $@ $^

clean:
	rm -f $(GARBAGE) $(EXECUTABLES)
