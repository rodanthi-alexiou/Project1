#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>
#include <random>

#include "LSHImplementation.h" 

struct fileData; 


std::vector<unsigned char*> kmeans_plusplus_init(const fileData& data, int K);
double euclideanDistance(const unsigned char* point1, const unsigned char* point2, int size);
std::vector<int> assignToNearestCentroids(const fileData& data, const std::vector<unsigned char*>& centroids);
int assignToNearestCentroidLSH(LSHImplementation& lsh, fileData& data, int L, int k, std::vector<double>& t, std::vector<unsigned char*>& centroids, int queryImage);
void updateCentroidsMacQueen(const fileData& data, const std::vector<int>& assignments, int K, std::vector<unsigned char*>& new_centroids);

#endif
