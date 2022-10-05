#pragma once
#include <stdio.h>

FILE *statefile_fopen(const char *pathname, const char *mode);
int statefile_fclose(FILE *stream);
int statefile_fseek(FILE *stream, long offset, int whence);
size_t statefile_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t statefile_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
