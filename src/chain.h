#ifndef CHAIN_H
#define CHAIN_H

void scan_folder(const char *dir);
void print_history(const char *filter);
int restore_file(const char *path, const char *hash);

#endif
