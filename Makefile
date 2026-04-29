CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lcrypto

TARGET = filechain

SRC = \
	src/main.c \
	src/cli.c \
	src/commands.c \
	src/file_utils.c \
	src/hash.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf .filechain
