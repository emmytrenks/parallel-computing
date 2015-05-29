#include <float.h>
#include <math.h>
#include <stdio.h>

__global__ void calculateAreas(const int recs, const double w, const int offset, double *areas) {
  const int index = threadIdx.x + offset;
  if (index >= recs) return;
  const double x = index * w;
  double h = 1 - x * x;
  h = h < DBL_EPSILON ? 0 : sqrt(h);
  areas[index] = w * h;
}

void calculateArea(const int recs, double *area) {
  double *areas = (double*) malloc(recs * sizeof(double));
  if (areas == NULL) {
    fprintf(stderr, "malloc failed!\n");
    return;
  }
  double *w_areas;
  cudaError_t err = cudaMalloc((void**) &w_areas, (recs * sizeof(double)));
  if (err != cudaSuccess) {
    fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(err));
    return;
  }
  const int threadCount = 512, loops = ceil((double) recs / threadCount);
  const double width = 1.0 / recs;
  for (int c = 0; c < loops; ++c) {
    calculateAreas<<<1, threadCount>>>(recs, width, c * threadCount, w_areas);
  }
  err = cudaMemcpy(areas, w_areas, recs * sizeof(double), cudaMemcpyDeviceToHost);
  if (err != cudaSuccess) {
    fprintf(stderr, "cudaMemcpy failed: %s\n", cudaGetErrorString(err));
    return;
  }
  *area = 0;
  for (int c = 0; c < recs; ++c) {
    *area += areas[c];
  }
  *area *= 4;
  cudaFree(w_areas);
  free(areas);
}
