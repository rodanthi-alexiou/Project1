#ifndef LSHIMPLEMENTATION_H
#define LSHIMPLEMENTATION_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

struct fileData {
    int magic_number;
    int number_of_images;
    int n_rows;
    int n_cols;
    int image_size;
    unsigned char** images;
};

class LSHImplementation {
    public: 

        // Define your global variables and data structures
        ifstream file;
        ifstream qFile;
        static vector<vector<vector<long long unsigned int>>> Euclidian_Hash_Tables;
        static vector<vector<double>> v;
        static vector<vector<int>> Euclidian_Amplified_Functions;
        static vector<double> t;
        static int w;
        static int k;
        static int L;

        const std::vector<std::vector<std::vector<long long unsigned int>>>& getEuclidianHashTables() const;
        const std::vector<std::vector<int>>& getEuclidianAmplifiedFunctions();
        // Getter function for amp_func
        int getAmplifiedFunction(int j, int k);

        // create a constructor for the class
        LSHImplementation(int L, int k, int w);
        
        // return L, k, w
        int getL();
        int getK();
        int getW();



        // Function declarations
        double normal_distribution_generator();
        long long int modulo(long long int a, long long int b);
        long long unsigned int euclidian_hash_query(int query_line, int amp_func, int k, fileData& qdata, const vector<double>& t);
        long long int calcute_euclidian_distance(int input_line, int query_line, fileData& fdata, fileData& qdata);
        void Euclidian_put_hash(int line, int L, int k, fileData& data, const vector<double>& t);
        unsigned char** read_mnist_images(ifstream& file, fileData& data);
        unsigned char* getImageLine(const fileData& data, int imageIndex);
        void initializeVectors(int k, int dim);
        void initializeAmplifiedFunctions(int L, int k);
        void initializeT(int k, int w);
        void cleanupHashTables(int L, fileData& data, int n);
        void initializeLSH(int L, int n, int k, int dim, int w);
        int FindNearestNeighbor(int query_line, int L, int k, fileData& data, fileData& qData, vector<double>& t);
        void Euclidian_Full_Search_NN(int query_line, fileData& data, fileData& qdata);
        void Euclidian_Full_Search_Range(int query_line, fileData& data, fileData& qdata, double radius);


        // Function declarations for the second part of the project
        long long unsigned int euclidian_hash_centroid(unsigned char* query_line, int amp_func, int k, fileData& data, vector<double>& t);
        int FindNearestNeighborCentroid(unsigned char* query_line, int L, int k, fileData& data, vector<double>& t);
        void insertCentroidIntoLSH(int index, int number_centroids, int L, int k, const std::vector<double>& t, const std::vector<unsigned char*>& centroid);

};

#endif  // LSHIMPLEMENTATION_H
