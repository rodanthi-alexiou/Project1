#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <climits>
#include <cassert>
#include <iomanip>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>
#include <cstring>
#define BIG_INT 4294967291
#define DBL_MAX 1.79769e+308


#include "LSHImplementation.h"


using namespace std;

typedef unsigned int uint;

ifstream file; /* Input File */


// Structure to represent a cluster
struct Cluster {
    std::vector<unsigned char*> points;  // Data points in the cluster
    unsigned char* centroid;           // Centroid of the cluster
};

unsigned char* computeCentroid(const Cluster& cluster, int imageSize) {
    unsigned char* centroid = new unsigned char[imageSize];
    std::fill(centroid, centroid + imageSize, 0);

    if (!cluster.points.empty()) {
        for (const unsigned char* point : cluster.points) {
            for (int i = 0; i < imageSize; ++i) {
                centroid[i] += point[i];
            }
        }

        for (int i = 0; i < imageSize; ++i) {
            centroid[i] /= cluster.points.size();
        }
    }

    return centroid;
}


// Function to print the clusters
void printClusters(const std::vector<Cluster>& clusters, double clusteringTime, const fileData& data) {
    int K = clusters.size();
    
    for (int k = 0; k < K; ++k) {
        const Cluster& cluster = clusters[k];
        int clusterSize = cluster.points.size();
        unsigned char* centroid = computeCentroid(cluster, data.image_size);

        // Print Cluster-k information
        std::cout << "CLUSTER-" << (k + 1) << " {size: " << clusterSize << ", centroid: [";
        for (int i = 0; i < data.image_size; ++i) {
            std::cout << static_cast<int>(centroid[i]);
            if (i < data.image_size - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "}" << std::endl;

        // Don't forget to free the centroid memory
        delete[] centroid;
    }

    // Print clustering time
    std::cout << "clustering_time: " << clusteringTime << " seconds" << std::endl;
}



// K-Means++ Initialization
std::vector<unsigned char*> kmeans_plusplus_init(const fileData& data, int K) {
    // Initialize a random number generator
    std::default_random_engine generator(static_cast<unsigned>(time(0)));

    std::vector<unsigned char*> centroids;
    centroids.reserve(K);

    // Choose the first centroid randomly
    std::uniform_int_distribution<int> distribution(0, data.number_of_images - 1);
    int first_centroid_idx = distribution(generator);
    centroids.push_back(data.images[first_centroid_idx]);

    // Calculate the squared distances from each data point to the nearest centroid
    std::vector<double> min_distances(data.number_of_images, DBL_MAX);

    for (int k = 1; k < K; ++k) {
        // Update min_distances for all data points
        for (int i = 0; i < data.number_of_images; ++i) {
            double min_dist = DBL_MAX;
            for (int j = 0; j < k; ++j) {
                double dist = 0;
                for (int p = 0; p < data.image_size; ++p) {
                    int diff = data.images[i][p] - centroids[j][p];
                    dist += diff * diff;
                }
                min_dist = std::min(min_dist, dist);
            }
            min_distances[i] = min_dist;
        }
         // Choose the next centroid with probability proportional to squared distance
        double total_dist = 0;
        for (int i = 0; i < data.number_of_images; ++i) {
            total_dist += min_distances[i];
        }

        double threshold = std::uniform_real_distribution<double>(0, total_dist)(generator);
        double cumulative_dist = 0;
        int new_centroid_idx = -1;
        for (int i = 0; i < data.number_of_images; ++i) {
            cumulative_dist += min_distances[i];
            if (cumulative_dist >= threshold) {
                new_centroid_idx = i;
                break;
            }
        }

        if (new_centroid_idx != -1) {
            centroids.push_back(data.images[new_centroid_idx]);
        }
    }

    return centroids;
}

// Function to update centroids using the MacQueen method
std::vector<unsigned char*> updateCentroids(const fileData& data, const std::vector<int>& assignments, int K) {
    int numDataPoints = data.number_of_images;
    int image_size = data.image_size;

    std::vector<unsigned char*> newCentroids(K, nullptr);

    // Initialize new centroids with zeros
    for (int k = 0; k < K; ++k) {
        newCentroids[k] = new unsigned char[image_size];
        std::fill(newCentroids[k], newCentroids[k] + image_size, 0);
    }

    std::vector<int> clusterSizes(K, 0);

    // Calculate the sums for each cluster
    for (int i = 0; i < numDataPoints; ++i) {
        int clusterIndex = assignments[i];
        clusterSizes[clusterIndex]++;
        for (int j = 0; j < image_size; ++j) {
            newCentroids[clusterIndex][j] += data.images[i][j];
        }
    }

    // Divide by cluster sizes to compute the means
    for (int k = 0; k < K; ++k) {
        for (int j = 0; j < image_size; ++j) {
            if (clusterSizes[k] > 0) {
                newCentroids[k][j] /= clusterSizes[k];
            }
        }
    }

    return newCentroids;
}


// Function to calculate the Euclidean distance between two data points
double euclideanDistance(const unsigned char* point1, const unsigned char* point2, int size) {
    double distance = 0;
    for (int i = 0; i < size; ++i) {
        int diff = point1[i] - point2[i];
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

// Assignment Step (Lloyds Algorithm)
std::vector<int> assignToNearestCentroids(const fileData& data, const std::vector<unsigned char*>& centroids) {
    int numDataPoints = data.number_of_images;
    int numCentroids = centroids.size();

    std::vector<int> assignments(numDataPoints);

    cout << "numDataPoints: " << numDataPoints << endl;

    for (int i = 0; i < numDataPoints; ++i) {
        double minDistance = std::numeric_limits<double>::max();
        int nearestCentroid = -1;

        for (int j = 0; j < numCentroids; ++j) {
            double distance = euclideanDistance(data.images[i], centroids[j], data.image_size);

            if (distance < minDistance) {
                minDistance = distance;
                nearestCentroid = j;
                }
        }

        assignments[i] = nearestCentroid;
    }

    return assignments;
}



// Assignment Step (LSH Algorithm)
//Function to assign a query point to its nearest centroid

int assignToNearestCentroidLSH(LSHImplementation& lsh, fileData& data, int L, int k, vector<double>& t, vector<unsigned char*>& centroids, int queryImage) {
    int nearestCentroidIndex = -1;
    long long unsigned int minDistance = numeric_limits<long long unsigned int>::max();

    // Calculate the hash value for the query image
    long long unsigned int queryHash = lsh.euclidian_hash_query(queryImage, lsh.L, lsh.k, data, lsh.t);

    cout << queryHash << endl;

    // Compare the query hash to the hash values of centroids
    for (int i = 0; i < centroids.size(); ++i) {
        long long unsigned int centroidHash = lsh.euclidian_hash_centroid(centroids[i], lsh.L, lsh.k, data, lsh.t);
        long long unsigned int distance = (queryHash - centroidHash) * (queryHash - centroidHash);

        if (distance < minDistance) {
            minDistance = distance;
            nearestCentroidIndex = i;
        }
    }

    return nearestCentroidIndex;
}


void updateCentroidsMacQueen(const fileData& data, const std::vector<int>& assignments, int K, std::vector<unsigned char*>& new_centroids) {
    std::vector<int> cluster_sizes(K, 0);

    for (int i = 0; i < data.number_of_images; ++i) {
        int cluster = assignments[i];
        cluster_sizes[cluster]++;
        if (new_centroids[cluster] == nullptr) {
            new_centroids[cluster] = new unsigned char[data.image_size];
        }

        for (int j = 0; j < data.image_size; ++j) {
            new_centroids[cluster][j] += data.images[i][j];
        }
    }

    for (int cluster = 0; cluster < K; ++cluster) {
        if (cluster_sizes[cluster] > 0) {
            for (int j = 0; j < data.image_size; ++j) {
                new_centroids[cluster][j] /= cluster_sizes[cluster];
            }
        }
    }
}






int main() {

    int algo = 1; // 0 for LSH, 1 for Lloyd's

    std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    
    fileData data = {
        0,0,0,0,0,nullptr  
    };

    LSHImplementation lsh(5, 4, 400);

    unsigned char** images = lsh.read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;

    // Number of clusters (you can set this)
    int K = 10;

    // Perform K-Means++ initialization
    std::vector<unsigned char*> initial_centroids = kmeans_plusplus_init(data, K);

    std::vector<int> assignments;

    if(algo == 0){

        int dim = data.image_size; // Dimension of the data
        vector<double> t(lsh.k);

        lsh.initializeLSH(lsh.L, data.number_of_images, lsh.k, dim, lsh.w);

        // Initialize the hash tables with the centroids
        for (int i = 0; i < initial_centroids.size(); ++i) {
            lsh.insertCentroidIntoLSH(i, initial_centroids.size(), lsh.L, lsh.k, t, initial_centroids);
        }

        lsh.cleanupHashTables(lsh.L, data, data.number_of_images);


        for (int i = 0; i < data.number_of_images; ++i) {
            unsigned char* query_line = lsh.getImageLine(data, i);
            int min_line = lsh.FindNearestNeighborCentroid(query_line, lsh.L, lsh.k, data, t);
            assignments.push_back(min_line);
        }


    }
    else if(algo == 1){
        assignments = assignToNearestCentroids(data, initial_centroids);
    }



    // After you've performed K-Means++ initialization and assignments
    std::vector<Cluster> clusters(K); // Initialize clusters



    for (int i = 0; i < data.number_of_images; ++i) {
        int clusterIndex = assignments[i]; // The assigned cluster index for data point i
        clusters[clusterIndex].points.push_back(data.images[i]); // Add the data point to the cluster
    }


    // Print size of each cluster
    for (int i = 0; i < K; i++) {
        cout << "Cluster " << i << " size: " << clusters[i].points.size() << endl;
    }



    

}

