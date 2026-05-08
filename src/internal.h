#ifndef INTERNAL_H
#define INTERNAL_H

#include "util.h"

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

void ensure_db(void);
void push(struct list *list, const char *path, const char *hash);
int find(struct list *list, const char *path);
void load_index(struct list *list);
void save_index(struct list *list);
void save_object(const char *path, const char *hash);
int restore_object(const char *path, const char *hash);
void add_block(const char *path, const char *data_hash);

#endif
