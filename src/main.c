#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

void print_usage(void)
{
    printf("Usage:\n");
    printf("  ./filechain hash <file>\n");
}

int read_file(const char *file_path, unsigned char **buffer, long *file_size)
{
    FILE *file = fopen(file_path, "rb");

    if (file == NULL)
    {
        printf("Error: could not open file: %s\n", file_path);
        return 1;
    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);

    if (size < 0)
    {
        printf("Error: could not get file size: %s\n", file_path);
        fclose(file);
        return 1;
    }

    rewind(file);

    unsigned char *data = malloc(size);

    if (data == NULL)
    {
        printf("Error: could not allocate memory.\n");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(data, 1, size, file);

    if (bytes_read != (size_t)size)
    {
        printf("Error: could not read entire file: %s\n", file_path);
        free(data);
        fclose(file);
        return 1;
    }

    fclose(file);

    *buffer = data;
    *file_size = size;

    return 0;
}

void print_sha256(const unsigned char *hash)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }

    printf("\n");
}

int hash_file(const char *file_path)
{
    unsigned char *buffer = NULL;
    long file_size = 0;

    int read_result = read_file(file_path, &buffer, &file_size);

    if (read_result != 0)
    {
        return 1;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(buffer, file_size, hash);

    printf("SHA-256: ");
    print_sha256(hash);

    free(buffer);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "hash") == 0)
    {
        if (argc < 3)
        {
            printf("Error: missing file path.\n");
            print_usage();
            return 1;
        }

        return hash_file(argv[2]);
    }

    printf("Error: unknown command: %s\n", argv[1]);
    print_usage();

    return 1;
}
