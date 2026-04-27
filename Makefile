CFLAGS = -Wall -Wextra -std=c11

all: filechain

filechain: src/main.c
	gcc $(CFLAGS) src/main.c -o filechain

run: filechain
	./filechain

clean:
	rm -f filechain
