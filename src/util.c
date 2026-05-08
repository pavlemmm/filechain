#include "util.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>

void sha256_hex(const void *data, size_t size, char out[65]) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  static const char hex[] = "0123456789abcdef";

  SHA256(data, size, hash);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    out[i * 2] = hex[hash[i] >> 4];
    out[i * 2 + 1] = hex[hash[i] & 15];
  }
  out[64] = 0;
}

char *read_file(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  char *buf;
  long n;

  if (!f)
    return NULL;
  if (fseek(f, 0, SEEK_END) != 0)
    return fclose(f), NULL;
  n = ftell(f);
  if (n < 0)
    return fclose(f), NULL;
  rewind(f);

  buf = malloc((size_t)n + 1);
  if (!buf)
    return fclose(f), NULL;
  if (n && fread(buf, 1, (size_t)n, f) != (size_t)n)
    return fclose(f), free(buf), NULL;

  buf[n] = 0;
  fclose(f);
  if (size)
    *size = (size_t)n;
  return buf;
}
