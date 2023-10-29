CC = g++
modules = LSHImplementation.cpp
args = -o

clust:
	$(CC) clust.cpp $(modules) $(args) clust

clean:
	rm -rf clust