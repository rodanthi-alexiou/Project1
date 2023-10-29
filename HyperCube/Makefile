CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=hypercube.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hypercube

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run:
	./hypercube

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
