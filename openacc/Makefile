COMPILER=CC

EXECUTABLES=1-kernels-sum 2-manual-sum 3-heat-dissipation

all:
	make $(EXECUTABLES)

1-kernels-sum: 1-kernels-sum.c
	$(COMPILER) -o $@ $^
GARBAGE+=1-kernels-sum.cub 1-kernels-sum.ptx
2-manual-sum: 2-manual-sum.c
	$(COMPILER) -o $@ $^
GARBAGE+=2-manual-sum.cub 2-manual-sum.ptx
3-heat-dissipation: 3-heat-dissipation.c
	$(COMPILER) -o $@ $^
GARBAGE+=3-heat-dissipation.cub 3-heat-dissipation.ptx

clean:
	rm -f $(GARBAGE) $(EXECUTABLES)

clean-output:
	rm -f *.pbs.{o,e}*
