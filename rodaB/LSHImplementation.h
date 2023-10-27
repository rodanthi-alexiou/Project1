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

// Define your global variables and data structures
extern ifstream file;
extern ifstream qFile;
static vector<vector<vector<long long unsigned int>>> Euclidian_Hash_Tables;
static vector<vector<double>> v;
static vector<vector<int>> Euclidian_Amplified_Functions;
static vector<double> t;
static int w;
static int k=4;
static int L=5;



// Function declarations
double normal_distribution_generator();
long long int modulo(long long int a, long long int b);
long long unsigned int euclidian_hash_query(unsigned char* query_line, int amp_func, int k, fileData& qdata, const vector<double>& t);
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

#endif  // LSHIMPLEMENTATION_H
