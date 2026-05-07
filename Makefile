CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OPENSSL = $(shell pkg-config --cflags --libs openssl 2>/dev/null || echo -lcrypto)

TARGET = filechain

SRC = src/main.c src/chain.c src/util.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(OPENSSL)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf .filechain
