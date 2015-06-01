COMPILER=CC
CUDA_COMPILER=nvcc

EXECUTABLES=1-calc-pi-cuda

all:
	make $(EXECUTABLES)

1-calc-pi-cuda.cub: 1-calc-pi-cuda.cu
	$(CUDA_COMPILER)  -o $@ -c $^
GARBAGE+=1-calc-pi-cuda.cub

1-calc-pi-cuda: 1-calc-pi-cuda.cub 1-calc-pi.c
	$(COMPILER) -o $@ $^

clean:
	rm -f $(GARBAGE) $(EXECUTABLES)
