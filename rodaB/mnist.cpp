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

    LSHImplementation lsh(5, 4, 400);


	std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    fileData data = { 0,0,0,0,0,nullptr };
    unsigned char** images = lsh.read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;


    //////////////////////////////////////////

    // Initialize the Hash Tables

    int n = data.number_of_images; 
    int dim = data.image_size;


    // initialize t
    vector<double> t(lsh.k);

    // Initialize LSH components
    lsh.initializeLSH(lsh.L, n, lsh.k, dim, lsh.w);

    //////////////////////////////////////////////

    for(int index=0; index < data.number_of_images; index++){


        lsh.Euclidian_put_hash(index,lsh.L,lsh.k,data,lsh.t);

    }

    

    lsh.cleanupHashTables(lsh.L, data, n);

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
    unsigned char** qImages = lsh.read_mnist_images(qFile, qData);

    cout << "Magic number: " << qData.magic_number << endl;
    cout << "Number of images: " << qData.number_of_images << endl;
    cout << "Number of rows: " << qData.n_rows << endl;
    cout << "Number of cols: " << qData.n_cols << endl;
    cout << "Image Size: " << qData.image_size << endl;


    //////////////////////////////////////////

    for(int query_line=0; query_line<10; query_line++){

        auto start1 = std::chrono::high_resolution_clock::now();

        int min_line = lsh.FindNearestNeighbor(query_line, lsh.L, lsh.k, data, qData, t);

        auto finish1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed1 = finish1 - start1;

        cout << "Query " << query_line << endl;
        cout << "NN(lsh): Item " << min_line << endl;

        // Calculate the true nearest neighbor and distance
        lsh.Euclidian_Full_Search_NN(query_line, data, qData);

        cout << "tLSH: " << elapsed1.count() << endl;

        cout << "-------------------------------------" << endl;

    }

    

    return 0;
}