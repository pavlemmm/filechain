#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "commands.h"

static void print_usage(void)
{
    printf("Usage:\n");
    printf("  ./filechain init\n");
    printf("  ./filechain add <file>\n");
    printf("  ./filechain add .\n");
}

int run_cli(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        return command_init();
    }

    if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            printf("Error: missing add target.\n");
            print_usage();
            return 1;
        }

        return command_add(argv[2]);
    }

    printf("Error: unknown command: %s\n", argv[1]);
    print_usage();

    return 1;
}
