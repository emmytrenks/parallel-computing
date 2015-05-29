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
  #pragma acc data create(i, a1[0:sample], a2[0:sample], a3[0:sample]), copyin(sample), copy(sum)
  {
    #pragma acc parallel loop
    for (i = 0; i < sample; ++i) {
      a1[i] = a2[i] = 1;
    }

    #pragma acc parallel loop
    for (i = 0; i < sample; ++i) {
      a3[i] = a1[i] + a2[i];
    }

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
