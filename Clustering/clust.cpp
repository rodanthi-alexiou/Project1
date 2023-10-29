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


using namespace std;

typedef unsigned int uint;

ifstream file; /* Input File */


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


    int K = 10;
    std::vector<int> cluster_sizes(K, 0);


    std::vector<unsigned char*> initial_centroids = kmeans_plusplus_init(data, K);
    std::vector<unsigned char*> centroids = initial_centroids;

    std::vector<int> assignments;

    if(algo == 0){

        int dim = data.image_size; 
        vector<double> t(lsh.k);

        lsh.initializeLSH(lsh.L, data.number_of_images, lsh.k, dim, lsh.w);

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

            std::vector<int> assignments = assignToNearestCentroids(data, centroids);


                for (int i = 0; i < data.number_of_images; ++i) {
                    int cluster = assignments[i];
                    cluster_sizes[cluster]++;
                }



            // for (int cluster = 0; cluster < K; ++cluster) {
            //     cout << "CLUSTER-" << cluster << " {size: " << cluster_sizes[cluster] << ", centroid: [";

            //     unsigned char* centroid = centroids[cluster];
                
            //     // Assuming each coordinate is a double
            //     for (int dim = 0; dim < data.image_size; ++dim) {
            //         int coordinate = int(centroid[dim]);
            //         cout << coordinate;

            //         if (dim < data.image_size - 1) {
            //             cout << ", ";
            //         }
            //     }
            //     cout << "]" << endl;
            // }


    }



cout << "///////////////////" << endl;


            std::vector<unsigned char*> new_centroids(K, nullptr);
            updateCentroidsMacQueen(data, assignments, K, new_centroids);



            for (int cluster = 0; cluster < K; ++cluster) {
                cout << "CLUSTER-" << cluster << " {size: " << cluster_sizes[cluster] << ", centroid: [";

                unsigned char* centroid = new_centroids[cluster];

                for (int dim = 0; dim < data.image_size; ++dim) {
                    int coordinate = int(centroid[dim]);
                    cout << coordinate;

                    if (dim < data.image_size - 1) {
                        cout << ", ";
                    }
                }
                cout << "]" << endl;
            }




    

}

