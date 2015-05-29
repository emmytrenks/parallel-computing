#include "1-calc-pi-cuda.h"
#include <stdio.h>

int main(int argc, char **argv) {
  const int recs = 0x100000;
  double area = 0.0;
  calculateArea(recs, &area);
  printf("The value of PI is approx. %.16f.\n", area);
  return 0;
}
