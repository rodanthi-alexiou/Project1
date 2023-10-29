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

// Define a struct for command line arguments
struct CommandLineArguments {
    std::string inputFile;
    std::string outputFile;
    std::string method;  // Method: Classic OR LSH OR Hypercube
};

// Function to parse and use command line arguments
CommandLineArguments useArguments(int argc, char* argv[]) {
    CommandLineArguments args;
    int cur_arg = 1;

    if (argc < 7) {
        std::cout << "Usage: $./cluster -i <input file> -o <output file> -m <method: Classic OR LSH OR Hypercube>" << std::endl;
        exit(1);
    }

    while (cur_arg < argc) {
        if (strcmp(argv[cur_arg], "-i") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an input file after -i." << std::endl;
                exit(1);
            } else {
                args.inputFile = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-o") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an output file after -o." << std::endl;
                exit(1);
            } else {
                args.outputFile = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-m") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify the method (Classic, LSH, or Hypercube) after -m." << std::endl;
                exit(1);
            } else {
                args.method = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else {
            std::cout << "Unknown argument: " << argv[cur_arg] << std::endl;
            exit(1);
        }
    }

    // Check if the method is one of the allowed values
    if (args.method != "Classic" && args.method != "LSH" && args.method != "Hypercube") {
        std::cout << "Invalid method. Please specify Classic, LSH, or Hypercube after -m." << std::endl;
        exit(1);
    }

    return args;
}


int main(int argc, char *argv[]) {



    CommandLineArguments args = useArguments(argc, argv);
    int algo;

    if (args.method == "LSH") {
        algo = 0; // Use LSH algorithm
    }
    else if(args.method == "Classic"){
        algo = 1; // Use Classic algorithm
    }
    else if(args.method == "Hypercube"){
        algo = 2; // Use Hypercube algorithm
    }
    else{
        cout << "Invalid method. Please specify Classic, LSH, or Hypercube after -m." << endl;
        exit(1);
    }

    std::ifstream file(args.inputFile, std::ios::binary);
    
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

