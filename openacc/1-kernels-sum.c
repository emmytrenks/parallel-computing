#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  const int sample = 10000;
  size_t size = sample * sizeof(int);
  int *restrict a1, *restrict a2, *restrict a3;
  a1 = (int*) malloc(size);
  a2 = (int*) malloc(size);
  a3 = (int*) malloc(size);

  for (int i = 0; i < sample; ++i) {
    a1[i] = a2[i] = 1;
  }

  //Tell the compiler to attempt to use GPU acceleration -- if successful
  // by sure to copy in the full array a1 and a2, and to copy out a3.
  //Copy in and out send and extract memory from the GPU respectively.
  #pragma acc kernels copyin(a1[0:sample], a2[0:sample]), copyout(a3[0:sample])
  for (int i = 0; i < sample; ++i) {
    a3[i] = a1[i] + a2[i];
  }

  int sum = 0;
  for (int i = 0; i < sample; ++i) {
    sum += a3[i];
  }

  free(a1);
  free(a2);
  free(a3);

  printf("Sample size: %d | Sum: %d\n", sample, sum);
}
