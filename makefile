CC = g++
args = -o

mnist:
	$(CC) clust.cpp $(args) clust

clean:
	rm -rf clust