#include <omp.h>
#include <stdio.h>

int main(int argc, char **argv) {
  //This pragma tells the compiler to parallelize
  // the loop, running on threads as a first-come first-served
  // fashion.
  #pragma omp parallel for
  for (int i = 0; i < 64; ++i) {
    printf("[THREAD %2d of %d] Loop index: %d.\n",
      omp_get_thread_num() + 1, omp_get_num_threads(), i);
  }
}
