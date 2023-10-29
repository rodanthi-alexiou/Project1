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

typedef unsigned char uchar;

struct CommandLineArguments {
    std::string inputFile;
    std::string queryFile;
    int k = 4; // Default value for k
    int L = 5; // Default value for L
    std::string outputFile;
    int numNearest = 1; // Default value for number of nearest
    double radius = -1.0; // Default value for radius
};

CommandLineArguments useArguments(int argc, char* argv[]) {
    CommandLineArguments args;
    int cur_arg = 1;

    while (cur_arg < argc) {
        if (strcmp(argv[cur_arg], "-d") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an Input file after -d." << std::endl;
                exit(1);
            } else {
                args.inputFile = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-q") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify a Query file after -q." << std::endl;
                exit(1);
            } else {
                args.queryFile = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-k") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an integer value after -k." << std::endl;
                exit(1);
            } else {
                args.k = std::stoi(argv[cur_arg + 1]);
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-L") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an integer value after -L." << std::endl;
                exit(1);
            } else {
                args.L = std::stoi(argv[cur_arg + 1]);
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-ο") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an output file after -ο." << std::endl;
                exit(1);
            } else {
                args.outputFile = argv[cur_arg + 1];
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-Ν") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify an integer value after -Ν." << std::endl;
                exit(1);
            } else {
                args.numNearest = std::stoi(argv[cur_arg + 1]);
                cur_arg += 2;
            }
        } else if (strcmp(argv[cur_arg], "-R") == 0) {
            if (cur_arg == (argc - 1) || argv[cur_arg + 1][0] == '-') {
                std::cout << "Please specify a floating-point value after -R." << std::endl;
                exit(1);
            } else {
                args.radius = std::stod(argv[cur_arg + 1]);
                cur_arg += 2;
            }
        } else {
            std::cout << "Unknown argument: " << argv[cur_arg] << std::endl;
            exit(1);
        }
    }

    return args;
}




int main(int argc, char *argv[]) {



    CommandLineArguments args = useArguments(argc, argv);


    std::string inputFile = args.inputFile;
    std::string queryFile = args.queryFile;
    int k = args.k;
    int L = args.L;
    std::string outputFile = args.outputFile;
    int numNearest = args.numNearest;
    double radius = args.radius;


    LSHImplementation lsh(L, k, 400);



    std::ifstream file(inputFile, std::ios::binary);
    fileData data = { 0,0,0,0,0,nullptr };
    unsigned char** images = lsh.read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;


    //////////////////////////////////////////


    int n = data.number_of_images; 
    int dim = data.image_size;



    vector<double> t(lsh.k);


    lsh.initializeLSH(lsh.L, n, lsh.k, dim, lsh.w);

    //////////////////////////////////////////////

    for(int index=0; index < data.number_of_images; index++){


        lsh.Euclidian_put_hash(index,lsh.L,lsh.k,data,lsh.t);

    }

    

    lsh.cleanupHashTables(lsh.L, data, n);

    file.close();
    

    ///////////////////////////////////////////////////////////////


    std::ifstream qFile("t10k-images.idx3-ubyte", std::ios::binary);


    fileData qData = {
        0,0,0,0,0,nullptr  
    };


    unsigned char** qImages = lsh.read_mnist_images(qFile, qData);

    cout << "Magic number: " << qData.magic_number << endl;
    cout << "Number of images: " << qData.number_of_images << endl;
    cout << "Number of rows: " << qData.n_rows << endl;
    cout << "Number of cols: " << qData.n_cols << endl;
    cout << "Image Size: " << qData.image_size << endl;


    //////////////////////////////////////////


        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error opening output file: " << outputFile << std::endl;
            return 1;
        }


        std::streambuf* originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(outFile.rdbuf());

    for(int query_line=0; query_line< 100 ; query_line++){

        auto start1 = std::chrono::high_resolution_clock::now();

        int min_line = lsh.FindNearestNeighbor(query_line, lsh.L, lsh.k, data, qData, t);

        auto finish1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed1 = finish1 - start1;
        auto start2 = std::chrono::high_resolution_clock::now();

        outFile << "Query " << query_line << std::endl;
        outFile << "NN(lsh): Item " << min_line << std::endl;
        //outFile << "distanceLSH: " << min_dist << std::endl;

        lsh.Euclidian_Full_Search_NN(query_line, data, qData);

        auto finish2 = std::chrono::high_resolution_clock::now();
	    std::chrono::duration<double> elapsed2 = finish2 - start2;

        outFile << "tLSH: " << elapsed1.count() << std::endl;
        outFile << "tTrue: " << elapsed2.count() << endl;


        cout << "-------------------------------------" << endl;

    }

    

    return 0;
}