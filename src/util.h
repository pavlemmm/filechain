#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

void sha256_hex(const void *data, size_t size, char out[65]);
char *read_file(const char *path, size_t *size);

#endif
