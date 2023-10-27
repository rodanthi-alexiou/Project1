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

#include "LSHImplementation.h"



using namespace std;





int main(int argc, char *argv[]) {


	std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    fileData data = { 0,0,0,0,0,nullptr };
    unsigned char** images = read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;


    //////////////////////////////////////////

    // Initialize the Hash Tables

    int n = data.number_of_images; 
    int dim = data.image_size;
    int w = 4000;

    // initialize t
    vector<double> t(k);

    // Initialize LSH components
    initializeLSH(L, n, k, dim, w);

    //////////////////////////////////////////////

    for(int index=0; index < data.number_of_images; index++){


        Euclidian_put_hash(index,L,k,data,t);

    }

    

    cleanupHashTables(L, data, n);

    ///////////////////////////////////////////////////////////////

    // close file
    file.close();
    


    // Find the nearest neighbor of a query

    // open query file  
    std::ifstream qFile("t10k-images.idx3-ubyte", std::ios::binary);

    // initialize query file data
    fileData qData = {
        0,0,0,0,0,nullptr  
    };

    // read query file
    unsigned char** qImages = read_mnist_images(qFile, qData);

    cout << "Magic number: " << qData.magic_number << endl;
    cout << "Number of images: " << qData.number_of_images << endl;
    cout << "Number of rows: " << qData.n_rows << endl;
    cout << "Number of cols: " << qData.n_cols << endl;
    cout << "Image Size: " << qData.image_size << endl;


    //////////////////////////////////////////

    for(int query_line=0; query_line<10; query_line++){


        int min_line = FindNearestNeighbor(query_line, L, k, data, qData, t);

        auto finish1 = std::chrono::high_resolution_clock::now();


        cout << "Query " << query_line << endl;
        cout << "NN(lsh): Item " << min_line << endl;
        // cout << "distanceLSH: " << min_dist << endl;


    }

    

    return 0;
}