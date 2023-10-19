CC = gcc
CFLAGS = -Wall -std=c99
SOURCES = lsh_mnist.c
EXECUTABLE = lsh_mnist

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean
