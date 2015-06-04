#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define PRINT_ENABLED true
#define ACC_ENABLED true

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print(const bool, double *, const int, const int, const double);
int parseArguments(int, char **, int *, bool *, int *, int *, double *, bool *, int *);

int main(int argc, char **argv) {
  /* Parameter defaults */
  int timeSteps = 5;//Number of time steps in heat dissipation
  bool printSteps = false;//Print each step, or just result
  int metalWidth = 5, metalHeight = 10;//Section of metal width and height
  double heaterTemp = 100.0;//The heating element temperature
  int delay_ns = 0;
  bool color = false;//Use color (only suggested for terminal display)
  if (parseArguments(argc, argv, &timeSteps, &printSteps, &metalWidth, &metalHeight, &heaterTemp, &color, &delay_ns)) return 1;
  struct timespec delay;
  delay.tv_sec = 0;
  delay.tv_nsec = delay_ns;
  //We need to bound the metal with insulation, and we know this adds
  // two rows and columns
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
  #if PRINT_ENABLED
  print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
  #endif
  double *metal = (double *) malloc(METAL_LEN * sizeof(double));
  int i;
  //We need to copy in and out our environment; however, metal
  // is strictly use for calculations, thus it is safe to create it
  // (create allocates the memory without copying host information
  // remember that this memory may have residue and needs set before
  // utilization)
  #if ACC_ENABLED
  #pragma acc data copy(environment[0:ENVIRONMENT_LEN]), create(metal[0:METAL_LEN], i)
  #endif
  {
    #if ACC_ENABLED
    #pragma acc parallel loop
    #endif
    for (i = 0; i < METAL_LEN; ++i) {
      const int r = i / metalWidth, c = i % metalWidth,
        index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
      metal[i] = environment[index];
    }
    for (int c = 0; c < timeSteps; c++) {
      #if ACC_ENABLED
      #pragma acc parallel loop
      #endif
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

      #if ACC_ENABLED
      #pragma acc parallel loop
      #endif
      for (i = 0; i < METAL_LEN; ++i) {
        const int r = i / metalWidth, c = i % metalWidth,
          index = 1 + (r + 1) * ENVIRONMENT_WIDTH + c;
        environment[index] = metal[i];
      }
      #if PRINT_ENABLED
      if (printSteps) {
        #if ACC_ENABLED
        #pragma acc update host(environment[0:ENVIRONMENT_LEN])
        #endif
        {
          print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
        }
        nanosleep(&delay, NULL);
      }
      #endif
    }
  }
  #if PRINT_ENABLED
  if (!printSteps) {
    print(color, environment, ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT, heaterTemp);
  }
  #endif
  //Free dynamicly allocated arrays
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
      //1000000 nano seconds in a millisecond
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
