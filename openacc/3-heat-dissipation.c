#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print(const bool, double *, const int, const int, const double);
int parseArguments(int, char **, int *, bool *, int *, int *, double *, bool *, int *);

int main(int argc, char **argv) {
  int timeSteps = 5;
  bool printSteps = false;
  int metalWidth = 5, metalHeight = 10;
  double heaterTemp = 100.0;
  int delay_ns = 0;
  bool color = false;
  if (parseArguments(argc, argv, &timeSteps, &printSteps, &metalWidth, &metalHeight, &heaterTemp, &color, &delay_ns)) return 1;
  struct timespec delay;
  delay.tv_sec = 0;
  delay.tv_nsec = delay_ns;
  const int ENVIRONMENT_WIDTH = metalWidth + 2, ENVIRONMENT_HEIGHT = metalHeight + 2;
  const int METAL_LEN = metalWidth * metalHeight, ENVIRONMENT_LEN = ENVIRONMENT_WIDTH * ENVIRONMENT_HEIGHT;
  double *environment = (double *) malloc(ENVIRONMENT_LEN * sizeof(double));
  #pragma omp parallel for
  for (int i = 0; i < ENVIRONMENT_LEN; ++i) {
    const int r = i / ENVIRONMENT_WIDTH, c = i % ENVIRONMENT_WIDTH;
    if (c == 0 || r == 0) {
      environment[i] = heaterTemp;
    } else {
      environment[i] = 0;
    }
  }
  print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
  double *metal = (double *) malloc(METAL_LEN * sizeof(double));
  int i;
  #pragma acc data copy(environment[0:ENVIRONMENT_LEN]), create(metal[0:METAL_LEN], i)
  {
    #pragma acc parallel loop
    for (i = 0; i < METAL_LEN; ++i) {
      const int r = i / metalWidth, c = i % metalWidth,
        index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
      metal[i] = environment[index];
    }
    for (int c = 0; c < timeSteps; c++) {
      #pragma acc parallel loop
      for (i = 0; i < METAL_LEN; ++i) {
        const int r = i / metalWidth, c = i % metalWidth,
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
        const int r = i / metalWidth, c = i % metalWidth,
          index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
        environment[index] = metal[i];
      }
      if (printSteps) {
        #pragma acc update host(environment[0:ENVIRONMENT_LEN])
        {
          print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
        }
        nanosleep(&delay, NULL);
      }
    }
  }
  if (!printSteps) {
    print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
  }
  free(environment);
  free(metal);
}

void print(const bool color, double *arr, const int w, const int h, const double maxTemp) {
  for (int i = 0; i < w * h; ++i) {
    const int r = i / w, c = i % w;
    if (r != 0 && c == 0) puts("");
    char buffer[32];
    if (arr[i] >= maxTemp * 0.9) {
      sprintf(buffer, "%s%.3f%s", (color ? ANSI_COLOR_RED : ""), arr[i], (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.75) {
      sprintf(buffer, "%s%.3f%s", (color ? ANSI_COLOR_MAGENTA : ""), arr[i], (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.5) {
      sprintf(buffer, "%s%.3f%s", (color ? ANSI_COLOR_YELLOW : ""), arr[i], (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.25) {
      sprintf(buffer, "%s%.3f%s", (color ? ANSI_COLOR_CYAN : ""), arr[i], (color ? ANSI_COLOR_RESET : ""));
    } else {
      sprintf(buffer, "%s%.3f%s", (color ? ANSI_COLOR_BLUE : ""), arr[i], (color ? ANSI_COLOR_RESET : ""));
    }
    printf((color ? "[%16s]" : "[%7s] "), buffer);
  }
  puts("\n");
}

int parseArguments(int argc, char **argv, int *timeSteps, bool *printSteps, int *width, int *height, double *temp, bool *color, int *delay) {
  int c;
  while ((c = getopt(argc, argv, "s:t:w:h:pcd:")) != -1) {
    switch (c) {
    case 's': {
      *timeSteps = atoi(optarg);
      break;
    }
    case 't': {
      *temp = atof(optarg);
      break;
    }
    case 'w': {
      *width = atoi(optarg);
      break;
    }
    case 'h': {
      *height = atoi(optarg);
      break;
    }
    case 'p': {
      *printSteps = true;
      break;
    }
    case 'c': {
      *color = true;
      break;
    }
    case 'd': {
      *delay = atoi(optarg) * 1000000;
      break;
    }
    default: {
      return 1;
    }
    }
  }
  return 0;
}
