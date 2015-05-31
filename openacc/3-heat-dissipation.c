#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print(const int, double *, const int, const int, const double);
int parseArguments(int, char **, int *, int *, int *, int *, double *, int *);

int main(int argc, char **argv) {
  int timeSteps = 5, printSteps = 0;
  int metalWidth = 5, metalHeight = 10;
  double heaterTemp = 100.0;
  int color = 0;
  if (parseArguments(argc, argv, &timeSteps, &printSteps, &metalWidth, &metalHeight, &heaterTemp, &color)) return 1;
  const int ENVIRONMENT_WIDTH = metalWidth + 2, ENVIRONMENT_HEIGHT = metalHeight + 2;
  const int METAL_LEN = metalWidth * metalHeight, ENVIRONMENT_LEN = ENVIRONMENT_WIDTH * ENVIRONMENT_HEIGHT;
  double *environment = malloc(ENVIRONMENT_LEN * sizeof(double));
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
  double *metal = malloc(METAL_LEN * sizeof(double));
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
        print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
      }
    }
  }
  if (!printSteps) {
    print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
  }
  free(environment);
  free(metal);
}

void print(const int color, double *arr, const int w, const int h, const double maxTemp) {
  for (int i = 0; i < w * h; ++i) {
    const int r = i / w, c = i % w;
    if (r != 0 && c == 0) puts("");
    char buffer[32];
    if (arr[i] >= maxTemp * 0.9) {
      sprintf(buffer, "%s%.3f%s", arr[i], (color ? ANSI_COLOR_RED : ""), (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.75) {
      sprintf(buffer, "%s%.3f%s", arr[i], (color ? ANSI_COLOR_MAGENTA : ""), (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.5) {
      sprintf(buffer, "%s%.3f%s", arr[i], (color ? ANSI_COLOR_YELLOW : ""), (color ? ANSI_COLOR_RESET : ""));
    } else if (arr[i] >= maxTemp * 0.25) {
      sprintf(buffer, "%s%.3f%s", arr[i], (color ? ANSI_COLOR_CYAN : ""), (color ? ANSI_COLOR_RESET : ""));
    } else {
      sprintf(buffer, "%s%.3f%s", arr[i], (color ? ANSI_COLOR_BLUE : ""), (color ? ANSI_COLOR_RESET : ""));
    }
    printf((color ? "[%16s]" : "[%7s] "), buffer);
  }
  puts("\n");
}

int parseArguments(int argc, char **argv, int *timeSteps, int *printSteps, int *width, int *height, double *temp, int *color) {
  int c;
  while ((c = getopt(argc, argv, "s:t:w:h:pc")) != -1) {
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
      *printSteps = 1;
      break;
    }
    case 'c': {
      *color = 1;
      break;
    }
    default: {
      return 1;
    }
    }
  }
  return 0;
}
