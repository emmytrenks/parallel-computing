#include <omp.h>
#include <stdio.h>

int main(int argc, char **argv) {
  #pragma omp parallel for
  for (int i = 0; i < 64; ++i) {
    printf("[THREAD %2d of %d] Loop index: %d.\n",
      omp_get_thread_num() + 1, omp_get_num_threads(), i);
  }
}
