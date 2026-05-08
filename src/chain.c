#include "chain.h"
#include "util.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define DB_DIR ".filechain"
#define OBJECT_DIR ".filechain/objects"
#define INDEX_FILE ".filechain/index.txt"
#define CHAIN_FILE ".filechain/chain.txt"

struct entry {
  char path[PATH_MAX];
  char hash[65];
};

struct list {
  struct entry *items;
  int len;
  int cap;
};

static void ensure_db(void) {
  struct stat st;

  if (stat(DB_DIR, &st) != 0)
    mkdir(DB_DIR, 0777);
  if (stat(OBJECT_DIR, &st) != 0)
    mkdir(OBJECT_DIR, 0777);
}

static void push(struct list *list, const char *path, const char *hash) {
  if (list->len == list->cap) {
    list->cap = list->cap ? list->cap * 2 : 16;
    list->items =
        realloc(list->items, (size_t)list->cap * sizeof(*list->items));
  }
  snprintf(list->items[list->len].path, PATH_MAX, "%s", path);
  snprintf(list->items[list->len].hash, sizeof(list->items[list->len].hash),
           "%s", hash);
  list->len++;
}

static int find(struct list *list, const char *path) {
  for (int i = 0; i < list->len; i++)
    if (strcmp(list->items[i].path, path) == 0)
      return i;
  return -1;
}

static void load_index(struct list *list) {
  FILE *f = fopen(INDEX_FILE, "r");
  char path[PATH_MAX];
  char hash[65];

  if (!f)
    return;
  while (fscanf(f, "%4095s %64s", path, hash) == 2)
    push(list, path, hash);
  fclose(f);
}

static void save_index(struct list *list) {
  FILE *f = fopen(INDEX_FILE, "w");

  if (!f)
    return;
  for (int i = 0; i < list->len; i++)
    fprintf(f, "%s %s\n", list->items[i].path, list->items[i].hash);
  fclose(f);
}

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

static void object_path(char out[PATH_MAX], const char *hash) {
  snprintf(out, PATH_MAX, "%s/%s", OBJECT_DIR, hash);
}

static void save_object(const char *path, const char *hash) {
  FILE *src;
  FILE *dst;
  FILE *test;
  char out[PATH_MAX];
  char buf[4096];
  size_t n;

  object_path(out, hash);
  test = fopen(out, "rb");
  if (test) {
    fclose(test);
    return;
  }

  src = fopen(path, "rb");
  dst = fopen(out, "wb");
  if (!src || !dst) {
    if (src)
      fclose(src);
    if (dst)
      fclose(dst);
    return;
  }

  while ((n = fread(buf, 1, sizeof(buf), src)) > 0)
    fwrite(buf, 1, n, dst);

  fclose(src);
  fclose(dst);
}

static void add_block(const char *path, const char *data_hash) {
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

static void scan_dir(const char *dir, struct list *old, struct list *now) {
  DIR *dp = opendir(dir);
  struct dirent *de;

  if (!dp)
    return;

  while ((de = readdir(dp))) {
    char path[PATH_MAX];
    struct stat st;
    char *content;
    size_t size;
    char hash[65];
    int i;

    if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") ||
        de->d_name[0] == '.')
      continue;

    snprintf(path, sizeof(path), "%s/%s", dir, de->d_name);
    if (stat(path, &st) != 0)
      continue;

    if (S_ISDIR(st.st_mode)) {
      scan_dir(path, old, now);
      continue;
    }
    if (!S_ISREG(st.st_mode))
      continue;

    content = read_file(path, &size);
    if (!content) {
      free(content);
      continue;
    }

    sha256_hex(content, size, hash);
    push(now, path, hash);

    i = find(old, path);
    if (i < 0 || strcmp(old->items[i].hash, hash) != 0) {
      save_object(path, hash);
      add_block(path, hash);
    }
    free(content);
  }

  closedir(dp);
}

void scan_folder(const char *dir) {
  struct list old = {0};
  struct list now = {0};

  ensure_db();
  load_index(&old);
  scan_dir(dir, &old, &now);
  save_index(&now);

  free(old.items);
  free(now.items);
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
