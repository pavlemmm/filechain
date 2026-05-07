#include "chain.h"

#include <stdio.h>
#include <string.h>

static void usage(const char *prog) {
  printf("Usage:\n");
  printf("  %s scan <folder>\n", prog);
  printf("  %s history [file]\n", prog);
}

int main(int argc, char **argv) {
  if (argc < 2)
    return usage(argv[0]), 0;

  if (strcmp(argv[1], "scan") == 0 && argc >= 3) {
    scan_folder(argv[2]);
    puts("Scan finished");
    return 0;
  }

  if (strcmp(argv[1], "history") == 0) {
    print_history(argc >= 3 ? argv[2] : NULL);
    return 0;
  }

  puts("Unknown command");
  return 1;
}
