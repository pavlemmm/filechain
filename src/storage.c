#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void ensure_db(void) {
  struct stat st;

  if (stat(DB_DIR, &st) != 0)
    mkdir(DB_DIR, 0777);
  if (stat(OBJECT_DIR, &st) != 0)
    mkdir(OBJECT_DIR, 0777);
}

void push(struct list *list, const char *path, const char *hash) {
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

int find(struct list *list, const char *path) {
  for (int i = 0; i < list->len; i++)
    if (strcmp(list->items[i].path, path) == 0)
      return i;
  return -1;
}

void load_index(struct list *list) {
  FILE *f = fopen(INDEX_FILE, "r");
  char path[PATH_MAX];
  char hash[65];

  if (!f)
    return;
  while (fscanf(f, "%4095s %64s", path, hash) == 2)
    push(list, path, hash);
  fclose(f);
}

void save_index(struct list *list) {
  FILE *f = fopen(INDEX_FILE, "w");

  if (!f)
    return;
  for (int i = 0; i < list->len; i++)
    fprintf(f, "%s %s\n", list->items[i].path, list->items[i].hash);
  fclose(f);
}

static void object_path(char out[PATH_MAX], const char *hash) {
  snprintf(out, PATH_MAX, "%s/%s", OBJECT_DIR, hash);
}

static int copy_file(const char *src_path, const char *dst_path) {
  FILE *src = fopen(src_path, "rb");
  FILE *dst = fopen(dst_path, "wb");
  char buf[4096];
  size_t n;

  if (!src || !dst) {
    if (src)
      fclose(src);
    if (dst)
      fclose(dst);
    return 1;
  }

  while ((n = fread(buf, 1, sizeof(buf), src)) > 0)
    fwrite(buf, 1, n, dst);

  fclose(src);
  fclose(dst);
  return 0;
}

void save_object(const char *path, const char *hash) {
  FILE *test;
  char out[PATH_MAX];

  object_path(out, hash);
  test = fopen(out, "rb");
  if (test) {
    fclose(test);
    return;
  }

  copy_file(path, out);
}

int restore_object(const char *path, const char *hash) {
  char object[PATH_MAX];

  object_path(object, hash);
  return copy_file(object, path);
}
