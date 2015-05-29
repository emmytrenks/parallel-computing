#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  const int sample = 10000;
  size_t size = sample * sizeof(int);
  int *restrict a1, *restrict a2, *restrict a3;
  a1 = (int*) malloc(size);
  a2 = (int*) malloc(size);
  a3 = (int*) malloc(size);
  int sum = 0;
  int i;
  //In this example, instead of a kernel pragma, we will define
  // ourselves what we would like to be parallelized.
  //We need to create i (iterator), a1, a2, and a3. We create rather
  // than copyin because the data is set once on the device, not
  // here on the host. Second, we copyin the sample size. Lastly, we
  // need to copy sum (copy is an alias for copyin and copyout).
  #pragma acc data create(i, a1[0:sample], a2[0:sample], a3[0:sample]), copyin(sample), copy(sum)
  {
    //Specify that this loop should run in parallel on the GPU.
    #pragma acc parallel loop
    for (i = 0; i < sample; ++i) {
      a1[i] = a2[i] = 1;
    }

    //Specify that this loop should run in parallel on the GPU.
    #pragma acc parallel loop
    for (i = 0; i < sample; ++i) {
      a3[i] = a1[i] + a2[i];
    }

    //Specify that this loop should run in parallel, but that reduction
    // is being performed on sum. This is vital, otherwise the addition
    // operation would not be atomic.
    #pragma acc parallel loop reduction(+:sum)
    for (i = 0; i < sample; ++i) {
      sum += a3[i];
    }
  }

  free(a1);
  free(a2);
  free(a3);

  printf("Sample size: %d | Sum: %d\n", sample, sum);
}
