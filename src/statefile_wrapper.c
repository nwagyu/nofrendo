#include "statefile_wrapper.h"

FILE * statefile_fopen(const char *pathname, const char *mode) {
  return NULL;
}

int statefile_fclose(FILE *stream) {
  return -1;
}

size_t statefile_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  return 0;
}

size_t statefile_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  return 0;
}

int statefile_fseek(FILE *stream, long offset, int whence) {
  return 0;
}
