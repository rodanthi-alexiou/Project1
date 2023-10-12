CC = gcc
CFLAGS = -Wall -std=c99
SOURCES = lsh.c
EXECUTABLE = lsh

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean
