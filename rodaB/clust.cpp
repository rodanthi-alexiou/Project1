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


using namespace std;

typedef unsigned int uint;

ifstream file; /* Input File */

struct fileData {
	int magic_number;
	int number_of_images;
	int n_rows;
	int n_cols;
	int image_size;
	unsigned char** images; 
};


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

unsigned char** read_mnist_images(std::ifstream& file, fileData& data) {
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;

    if (file.is_open()) {
        int magic_number = 0, n_rows = 0, n_cols = 0;
        int number_of_images, image_size;

        file.read((char*)&magic_number, sizeof(magic_number));
        magic_number = reverseInt(magic_number);

        if (magic_number != 2051) throw std::runtime_error("Invalid MNIST image file!");

        file.read((char*)&number_of_images, sizeof(number_of_images));
        number_of_images = reverseInt(number_of_images);
        file.read((char*)&n_rows, sizeof(n_rows));
        n_rows = reverseInt(n_rows);
        file.read((char*)&n_cols, sizeof(n_cols));
        n_cols = reverseInt(n_cols);

        image_size = n_rows * n_cols;

        data.magic_number = magic_number;
        data.number_of_images = number_of_images;
        data.n_rows = n_rows;
        data.n_cols = n_cols;
        data.image_size = image_size;
        data.images = new unsigned char*[data.number_of_images];

        uchar** _dataset = new uchar*[number_of_images];
        for(int i = 0; i < number_of_images; i++) {
            _dataset[i] = new uchar[image_size];
            file.read((char *)_dataset[i], image_size);
        }

        for (int i = 0; i < data.number_of_images; i++) {
            data.images[i] = new unsigned char[data.image_size];
            std::memcpy(data.images[i], _dataset[i], data.image_size);
        }

        return _dataset;
    } else {
        throw runtime_error("Cannot open file!");
    }
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



int main() {
    std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    
    fileData data = {
        0,0,0,0,0,nullptr  
    };

    unsigned char** images = read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;

    // Number of clusters (you can set this)
    int K = 10;

    // Perform K-Means++ initialization
    std::vector<unsigned char*> initial_centroids = kmeans_plusplus_init(data, K);

    // Assign data points to the nearest centroids using the Exact Panas Algorithm
    std::vector<int> assignments = assignToNearestCentroids(data, initial_centroids);


    // After you've performed K-Means++ initialization and assignments
    std::vector<Cluster> clusters(K); // Initialize clusters

    //int maxIterations = 10; // Set the maximum number of iterations

    for (int i = 0; i < data.number_of_images; ++i) {
        int clusterIndex = assignments[i]; // The assigned cluster index for data point i
        clusters[clusterIndex].points.push_back(data.images[i]); // Add the data point to the cluster
    }

    // Record the time taken for clustering (you can use <chrono> for this)
    auto startTime = std::chrono::high_resolution_clock::now();

    // for (int iteration = 0; iteration < maxIterations; ++iteration) {
    //     // Update the centroids using the MacQueen method
    //     std::vector<unsigned char*> updatedCentroids = updateCentroids(data, assignments, K);

    //     // Reassign data points to the updated centroids
    //     assignments = assignToNearestCentroids(data, updatedCentroids);

    //     // Clear the points in each cluster
    //     for (int k = 0; k < K; ++k) {
    //         clusters[k].points.clear();
    //     }

    //     // Update clusters with data point assignments
    //     for (int i = 0; i < data.number_of_images; ++i) {
    //         int clusterIndex = assignments[i];
    //         clusters[clusterIndex].points.push_back(data.images[i]);
    //     }
    // }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> clusteringDuration = endTime - startTime;

    // Print size of each cluster
    for (int i = 0; i < K; i++) {
        cout << "Cluster " << i << " size: " << clusters[i].points.size() << endl;
    }

    // Print the clusters and clustering time
    //printClusters(clusters, clusteringDuration.count(), data);

}

