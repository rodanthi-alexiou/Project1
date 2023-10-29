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
#include "Clustering.h"

// Αρχικοποίηση K-Means++
std::vector<unsigned char*> kmeans_plusplus_init(const fileData& data, int K) {
    // Αρχικοποίηση γεννήτριας τυχαίων αριθμών
    std::default_random_engine generator(static_cast<unsigned>(time(0)));

    std::vector<unsigned char*> centroids;
    centroids.reserve(K);

    // Επιλογή του πρώτου κέντρου τυχαία
    std::uniform_int_distribution<int> distribution(0, data.number_of_images - 1);
    int first_centroid_idx = distribution(generator);
    centroids.push_back(data.images[first_centroid_idx]);

    // Υπολογισμός τετραγωνικών αποστάσεων από κάθε σημείο δεδομένων προς το πλησιέστερο κέντρο
    std::vector<double> min_distances(data.number_of_images, DBL_MAX);

    for (int k = 1; k < K; ++k) {
        // Ενημέρωση των ελάχιστων αποστάσεων για όλα τα σημεία δεδομένων
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
         // Επιλογή του επόμενου κέντρου με πιθανότητα ανάλογη της τετραγωνικής απόστασης
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

// Υπολογισμός της Ευκλείδιας απόστασης μεταξύ δύο σημείων δεδομένων
double euclideanDistance(const unsigned char* point1, const unsigned char* point2, int size) {
    double distance = 0;
    for (int i = 0; i < size; ++i) {
        int diff = point1[i] - point2[i];
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

// Βήμα ανάθεσης (αλγόριθμος Lloyd)
std::vector<int> assignToNearestCentroids(const fileData& data, const std::vector<unsigned char*>& centroids) {
    int numDataPoints = data.number_of_images;
    int numCentroids = centroids.size();

    std::vector<int> assignments(numDataPoints);

    cout << "numDataPoints: " << numDataPoints << endl;
    cout << "numCentroids: " << numCentroids << endl;

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

// Βήμα ανάθεσης (αλγόριθμος LSH)
// Συνάρτηση για την ανάθεση ενός σημείου query στο πλησιέστερο κέντρο
int assignToNearestCentroidLSH(LSHImplementation& lsh, fileData& data, int L, int k, vector<double>& t, vector<unsigned char*>& centroids, int queryImage) {
    int nearestCentroidIndex = -1;
    long long unsigned int minDistance = numeric_limits<long long unsigned int>::max();

    // Υπολογισμός της τιμής του κατακερματισμού για την εικόνα query
    long long unsigned int queryHash = lsh.euclidian_hash_query(queryImage, lsh.L, lsh.k, data, lsh.t);

    cout << queryHash << endl;

    // Σύγκριση του κατακερματισμού του query με τους κατακερματισμούς των κέντρων
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

// Ενημέρωση των κέντρων χρησιμοποιώντας τη μέθοδο του MacQueen
void updateCentroidsMacQueen(const fileData& data, const std::vector<int>& assignments, int K, std::vector<unsigned char*>& new_centroids) {
    std::vector<int> cluster_sizes(K, 0);

    for (int i = 0; i < data.number_of_images; ++i) {
        int cluster = assignments[i];
        cluster_sizes[cluster]++;
        if (new_centroids[cluster] == nullptr) {
            new_centroids[cluster] = new unsigned char[data.image_size];
            // Αρχικοποίηση με 0
            std::fill(new_centroids[cluster], new_centroids[cluster] + data.image_size, 0);
        }

        for (int j = 0; j < data.image_size; ++j) {
            int intValue = static_cast<int>(data.images[i][j]);
            new_centroids[cluster][j] += static_cast<unsigned char>(intValue);
            // cout << static_cast<int>(new_centroids[cluster][j]) << endl;
        }

    }

    for (int cluster = 0; cluster < K; ++cluster) {
        if (cluster_sizes[cluster] > 0) {
            for (int j = 0; j < data.image_size; ++j) {
                int sum = static_cast<int>(new_centroids[cluster][j]);
                sum /= cluster_sizes[cluster];

                new_centroids[cluster][j] = static_cast<unsigned char>(sum);
                
                // cout << static_cast<int>(new_centroids[cluster][j]) << endl;
                int coordinate = int(new_centroids[cluster][j]);
                // cout << coordinate << endl;
            }
        }
    }
}
