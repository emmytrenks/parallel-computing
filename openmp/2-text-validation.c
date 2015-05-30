#include <omp.h>
#include <string.h>
#include <stdio.h>

int isAlphanumeric(const char *str) {
  const int size = strlen(str);
  #pragma omp parallel for
  for (int index = 0; index < size; ++index) {
    const char c = str[index];
    if (!(c >= '0' && c <= '9' ||
      c >= 'A' && c <= 'Z' ||
      c >= 'a' && c <= 'z')) return 0;
  }
  return 1;
}

int main(int argc, char **argv) {
  if (argc != 2) return 1;
  const char *str = argv[1];
  if (isAlphanumeric(str)) {
    printf("`%s` is alphanumeric.\n", str);
  } else {
    printf("`%s` is not alphanumeric.\n", str);
  }
  return 0;
}
