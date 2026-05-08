#include "chain.h"
#include "internal.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static void last_block_hash(char out[65]) {
  FILE *f = fopen(CHAIN_FILE, "r");
  char line[256];

  strcpy(out, "0");
  if (!f)
    return;
  while (fgets(line, sizeof(line), f))
    sscanf(line, "block_hash %64s", out);
  fclose(f);
}

void add_block(const char *path, const char *data_hash) {
  FILE *f = fopen(CHAIN_FILE, "a");
  char stamp[32];
  char prev[65];
  char meta[PATH_MAX + 200];
  char block_hash[65];
  time_t now = time(NULL);

  if (!f)
    return;

  last_block_hash(prev);
  strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
  snprintf(meta, sizeof(meta), "%s|%s|%s|%s", stamp, path, data_hash, prev);
  sha256_hex(meta, strlen(meta), block_hash);

  fprintf(f, "---\n");
  fprintf(f, "time %s\n", stamp);
  fprintf(f, "file %s\n", path);
  fprintf(f, "data_hash %s\n", data_hash);
  fprintf(f, "prev_hash %s\n", prev);
  fprintf(f, "block_hash %s\n", block_hash);
  fprintf(f, "object %s/%s\n", OBJECT_DIR, data_hash);
  fclose(f);
}

void print_history(const char *filter) {
  FILE *f = fopen(CHAIN_FILE, "r");
  char line[4096];
  char block[16384] = "";
  int match = !filter;

  if (!f) {
    puts("No history");
    return;
  }

  while (fgets(line, sizeof(line), f)) {
    if (strcmp(line, "---\n") == 0) {
      if (block[0] && match)
        printf("%s", block);
      block[0] = 0;
      match = !filter;
    }

    if (strncmp(line, "file ", 5) == 0) {
      line[strcspn(line, "\n")] = 0;
      match = !filter || strcmp(line + 5, filter) == 0;
      strcat(line, "\n");
    }

    if (strlen(block) + strlen(line) < sizeof(block) - 1)
      strcat(block, line);
  }

  if (block[0] && match)
    printf("%s", block);
  fclose(f);
}
