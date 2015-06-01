#include <omp.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

bool isAlphanumeric(const char *str) {
  const int size = strlen(str);
  bool result = true;
  #pragma omp parallel for
  for (int index = 0; index < size; ++index) {
    #pragma omp flush (result)
    if (!result) continue;

    const char c = str[index];
    if (!(c >= '0' && c <= '9' ||
      c >= 'A' && c <= 'Z' ||
      c >= 'a' && c <= 'z')) {
        //We don't need to specify atomic, because all
        // threads can compete to set it to one value.
        #pragma omp flush (result)
        result = false;
      }
  }
  return result;
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
