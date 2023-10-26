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



using namespace std;

ifstream file; /* Input File */

ifstream qFile; /* Query File */

static vector<vector<vector<long long unsigned int>>> Euclidian_Hash_Tables; /* Hash Tables */

static vector<vector<double> > v; /* Vectors V used in Hash Functions */

static vector< vector<int> > Euclidian_Amplified_Functions; /* Amplified Hash Functions */



struct fileData {
	int magic_number;
	int number_of_images;
	int n_rows;
	int n_cols;
	int image_size;
	std::vector<std::vector<double>> images; 
};

int k=4; /* Number of LSH Functions */

int L=5; /* Number of Hash Tables  */

double normal_distribution_generator(void); /* Returns a random number following normal distribution */

double normal_distribution_generator(void){

	/* Based on https://en.cppreference.com/w/cpp/numeric/random/normal_distribution */

	random_device rd{};
    mt19937 gen{rd()};

	normal_distribution<float> d{0,1};

	map<int, int> hist{};

	return round(d(gen));

}




std::vector<int> LSHHashFunction(const std::vector<double>& dataPoint, const std::vector<std::vector<double>>& v) {
    std::vector<int> hashValues(v.size(), 0);

    for (int i = 0; i < v.size(); i++) {
        double projection = 0.0;
        for (int j = 0; j < dataPoint.size(); j++) {
            projection += v[i][j] * dataPoint[j];
        }

        // Convert the projection to an unsigned int hash value
        hashValues[i] = static_cast<int>(projection);
    }

    return hashValues;
}



std::vector<std::vector<double>> read_mnist_images(std::ifstream& file, fileData& data) {
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

        std::vector<std::vector<double>> trainingData;

        for (int i = 0; i < number_of_images; i++) {
            std::vector<double> featureVector(image_size);

            for (int j = 0; j < image_size; j++) {
                uchar pixel;
                file.read((char*)&pixel, sizeof(uchar));
                featureVector[j] = static_cast<double>(pixel) / 255.0; // Normalize pixel values to [0, 1]
            }

            trainingData.push_back(featureVector);
        }

        data.images = trainingData;

        return trainingData;
    } else {
        throw std::runtime_error("Cannot open file!");
    }
}


int main(int argc, char *argv[]) {

    // string str;

    // 		cout << "Please specify an input file: ";

	// 	cin >> str;



	std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    
    fileData data = {
        0,     // magic_number
        0,     // number_of_images
        0,     // n_rows
        0,     // n_cols
        0,     // image_size
        std::vector<std::vector<double>>() // images (empty vector)
    };


    // unsigned char** images = read_mnist_images(file, data);

    std::vector<std::vector<double>> images = read_mnist_images(file, data);





///////////////////////////////////////////////

std::map<int, std::vector<double>> trainingDataWithImageNumbers;


    // Loop through each feature vector in the training data
    for (int imageNumber = 0; imageNumber < images.size(); imageNumber++) {
        // Calculate hash values for the feature vector
        std::vector<int> hashValues = LSHHashFunction(images[imageNumber], v);

        // Associate image number with feature vector
        trainingDataWithImageNumbers[imageNumber] = images[imageNumber];

        // Insert the feature vector into the hash tables
        for (int i = 0; i < L; i++) {
            int tableIndex = hashValues[i];
            if (tableIndex >= 0 && tableIndex < Euclidian_Hash_Tables[i].size()) {
                Euclidian_Hash_Tables[i][tableIndex].push_back(images[imageNumber]);
                cout << "Hash Table: " << i << " Bucket: " << tableIndex << endl;
                cout << "Image Number: " << imageNumber << endl;
            }
        }
    }




    //////////////////////////////////////////

    cout << "okay" << endl;
    cout << endl;

    // Perform the LSH Search

    // open query file  
    std::ifstream qFile("t10k-images.idx3-ubyte", std::ios::binary);

    // initialize query file data
    fileData qData = {
        0,     // magic_number
        0,     // number_of_images
        0,     // n_rows
        0,     // n_cols
        0,     // image_size
        std::vector<std::vector<double>>() // images (empty vector)
    };

    std::vector<std::vector<double>> queryImages = read_mnist_images(qFile, qData);

    cout << "Magic number: " << qData.magic_number << endl;
    cout << "Number of images: " << qData.number_of_images << endl;
    cout << "Number of rows: " << qData.n_rows << endl;
    cout << "Number of cols: " << qData.n_cols << endl;
    cout << "Image Size: " << qData.image_size << endl;

    //////////////////////////////////////////


    // Print the number of hash tables
    int numHashTables = Euclidian_Hash_Tables.size();
    std::cout << "Number of Hash Tables: " << numHashTables << std::endl;


    // Print the size of each hash table
    for (int i = 0; i < Euclidian_Hash_Tables.size(); i++) {
        int tableSize = Euclidian_Hash_Tables[i].size();
        std::cout << "Hash Table " << i << " Size: " << tableSize << std::endl;
    }

    // // Print the number of items in each hash table
    // for (int i = 0; i < Euclidian_Hash_Tables.size(); i++) {
    //     int tableIndex = 0;
    //     for (const std::vector<std::vector<double>>& bucket : Euclidian_Hash_Tables[i]) {
    //         tableIndex++;
    //         int itemCount = bucket.size();
    //         std::cout << "Hash Table " << i << " Bucket " << tableIndex << " Size: " << itemCount << std::endl;
    //     }
    // }





    /////////////////////////////////////////




    // get the first query image
    std::vector<double> queryImage = queryImages[2];

    std::vector<int> queryHashValues = LSHHashFunction(queryImage, v);

    std::vector<std::vector<std::vector<double>>> candidateBuckets(L);


    int queryBucketIndex = -1; // Initialize to an invalid value
    // Find candidate neighbors
    for (int i = 0; i < L; i++) {
        int tableIndex = queryHashValues[i];
        
        if (tableIndex >= 0 && tableIndex < Euclidian_Hash_Tables[i].size()) {
            // Retrieve feature vectors from the corresponding bucket
            candidateBuckets[i] = Euclidian_Hash_Tables[i][tableIndex];

                   // Store the index of the first valid bucket found
            if (queryBucketIndex == -1) {
                queryBucketIndex = i;
            }
            
        }
    }


    if (queryBucketIndex != -1) {
        std::cout << "Query Image is in the bucket of Hash Table " << queryBucketIndex << std::endl;
        int bucketSize = candidateBuckets[queryBucketIndex].size();
        std::cout << "Bucket Size: " << bucketSize << std::endl;

            if (!candidateBuckets[queryBucketIndex].empty()) {
                std::vector<double> firstItem = candidateBuckets[queryBucketIndex].front();
                std::cout << "First Item in the Bucket: ";
                for (double value : firstItem) {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
            } else {
                std::cout << "The bucket is empty." << std::endl;
            }

    } else {
        std::cout << "Query Image is not found in any valid bucket." << std::endl;
    }


    // Flatten the candidate buckets into a single vector
    std::vector<std::vector<double>> candidateNeighbors;

    for (int i = 0; i < L; i++) {
        candidateNeighbors.insert(candidateNeighbors.end(), candidateBuckets[i].begin(), candidateBuckets[i].end());
    }

// Now, candidateNeighbors contains the potential nearest neighbors for the queryImage


    // find the most nearest neighbor


    std::vector<double> nearestNeighbor;
    double minDistance = std::numeric_limits<double>::max();
    int nearestNeighborImageNumber = -1;

    for (int imageNumber = 0; imageNumber < candidateNeighbors.size(); imageNumber++) {
        const std::vector<double>& neighbor = candidateNeighbors[imageNumber];

        std::cout << "Size of the neighbor vector: " << neighbor.size() << std::endl;

        
        // Calculate the Euclidean distance between the query image and the candidate neighbor
        double distance = 0.0;
        cout << queryImage.size() << endl;
        for (int i = 0; i < queryImage.size(); i++) {
            double diff = queryImage[i] - neighbor[i];
            distance += diff * diff;
        }
        distance = std::sqrt(distance);

        cout << "okay " << endl;

        // Update the nearest neighbor if this neighbor is closer
        if (distance < minDistance) {
            minDistance = distance;
            nearestNeighbor = neighbor;
            nearestNeighborImageNumber = imageNumber; // Store the image number
        }
    }

    // Print the nearest neighbor's image number and distance
    std::cout << "Nearest Neighbor Image Number: " << nearestNeighborImageNumber << std::endl;
    std::cout << "Nearest Neighbor Distance: " << minDistance << std::endl;


