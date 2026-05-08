#include "chain.h"
#include "internal.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

int restore_file(const char *path, const char *hash) {
  return restore_object(path, hash);
}
