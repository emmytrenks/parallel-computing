#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print(double *, const int, const int);

int main(int argc, char **argv) {
  const int TIME_STEPS = argc >= 2 ? atoi(argv[1]) : 10;
  const int PRINT_STEPS = argc >= 3 ? (strstr(argv[2], "t") ? 1 : 0) : 0;
  const int METAL_WIDTH = argc >= 4 ? atoi(argv[3]) : 5, METAL_HEIGHT = argc >= 5 ? atoi(argv[4]) : 10;
  const int ENVIRONMENT_WIDTH = METAL_WIDTH + 2, ENVIRONMENT_HEIGHT = METAL_HEIGHT + 2;
  const int METAL_LEN = METAL_WIDTH * METAL_HEIGHT, ENVIRONMENT_LEN = ENVIRONMENT_WIDTH * ENVIRONMENT_HEIGHT;
  double *environment = malloc(ENVIRONMENT_LEN * sizeof(double));
  #pragma omp parallel for
  for (int i = 0; i < ENVIRONMENT_LEN; ++i) {
    const int r = i / ENVIRONMENT_WIDTH, c = i % ENVIRONMENT_WIDTH;
    if (c == 0 || r == 0) {
      environment[i] = 100;
    } else {
      environment[i] = 0;
    }
  }
  print(environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT);
  double *metal = malloc(METAL_LEN * sizeof(double));
  int i;
  #pragma acc data copy(environment[0:ENVIRONMENT_LEN]), create(metal[0:METAL_LEN], i)
  {
    #pragma acc parallel loop
    for (i = 0; i < METAL_LEN; ++i) {
      const int r = i / METAL_WIDTH, c = i % METAL_WIDTH,
        index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
      metal[i] = environment[index];
    }
    for (int c = 0; c < TIME_STEPS; c++) {
      #pragma acc parallel loop
      for (i = 0; i < METAL_LEN; ++i) {
        const int r = i / METAL_WIDTH, c = i % METAL_WIDTH,
          index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
        const int r2 = index / ENVIRONMENT_WIDTH, c2 = index % ENVIRONMENT_WIDTH;

        const int sourceL = (r2 - 1) * ENVIRONMENT_WIDTH + c2,
          sourceR = (r2 + 1) * ENVIRONMENT_WIDTH + c2,
          sourceU = r2 * ENVIRONMENT_WIDTH + (c2 - 1),
          sourceD = r2 * ENVIRONMENT_WIDTH + (c2 + 1);
        const double ourTemp =
          (environment[sourceL] +
          environment[sourceR] +
          environment[sourceU] +
          environment[sourceD]) / 4.0;
        metal[i] = ourTemp;
      }

      #pragma acc parallel loop
      for (i = 0; i < METAL_LEN; ++i) {
        const int r = i / METAL_WIDTH, c = i % METAL_WIDTH,
          index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
        environment[index] = metal[i];
      }
      if (PRINT_STEPS) {
        print(environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT);
      }
    }
  }
  if (!PRINT_STEPS) {
    print(environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT);
  }
  free(environment);
  free(metal);
}

void print(double *arr, const int w, const int h) {
  for (int i = 0; i < w * h; ++i) {
    const int r = i / w, c = i % w;
    if (r != 0 && c == 0) puts("");
    char buffer[32];
    if (arr[i] >= 90.0) {
      sprintf(buffer, ANSI_COLOR_RED "%.3f" ANSI_COLOR_RESET, arr[i]);
    } else if (arr[i] >= 75.0) {
      sprintf(buffer, ANSI_COLOR_MAGENTA "%.3f" ANSI_COLOR_RESET, arr[i]);
    } else if (arr[i] >= 50.0) {
      sprintf(buffer, ANSI_COLOR_YELLOW "%.3f" ANSI_COLOR_RESET, arr[i]);
    } else if (arr[i] >= 25.0) {
      sprintf(buffer, ANSI_COLOR_CYAN "%.3f" ANSI_COLOR_RESET, arr[i]);
    } else {
      sprintf(buffer, ANSI_COLOR_BLUE "%.3f" ANSI_COLOR_RESET, arr[i]);
    }
    printf("[%16s] ", buffer);
  }
  puts("\n");
}
