CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -g

.PHONY: all test sanitize clean
all: build/social-network

build:
	mkdir -p build

build/social-network: main.c a3_functions.c a3_functions.h a3_nodes.h | build
	$(CC) $(CFLAGS) main.c a3_functions.c -o $@

build/test-functions: tests/test_functions.c a3_functions.c a3_functions.h a3_nodes.h | build
	$(CC) $(CFLAGS) -I. tests/test_functions.c a3_functions.c -o $@

test: build/test-functions
	./build/test-functions
	./build/test-functions eof < /dev/null

sanitize: | build
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -I. tests/test_functions.c a3_functions.c -o build/test-sanitize
	./build/test-sanitize
	./build/test-sanitize eof < /dev/null

clean:
	rm -rf build
