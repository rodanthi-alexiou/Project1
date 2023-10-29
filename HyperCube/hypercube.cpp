#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <cstdint>
#include <fstream>

using namespace std;

                                     
const int IMAGE_SIZE = 28 * 28;         // Size of an MNIST image in pixels.


using BinaryPoint = vector<int>;        // A binary point is represented as a vector of integers (0 or 1).


BinaryPoint generateRandomBinaryPoint(int d) {  // Function to generate a random binary point of a given dimension.
    BinaryPoint p;
    for (int i = 0; i < d; i++) {
        p.push_back(rand() % 2);
    }
    return p;
}


BinaryPoint imageToBinaryVector(const unsigned char* image) {   // Function to convert an MNIST image to a binary vector.
    BinaryPoint binaryVector;
    for (int i = 0; i < IMAGE_SIZE; i++) {
        // Convert the pixel to binary (0 or 1).
        binaryVector.push_back(image[i] > 128 ? 1 : 0);  
    }
    return binaryVector;
}


vector<BinaryPoint> readMNISTImages(const string& filename, int numImages) {    // Function to read MNIST images from a file.
    vector<BinaryPoint> binaryVectors;
    ifstream file(filename, ios::binary);

    if (file.is_open()) {
        file.seekg(16);

        for (int i = 0; i < numImages; i++) {
            unsigned char image[IMAGE_SIZE];
            file.read(reinterpret_cast<char*>(image), IMAGE_SIZE);
            binaryVectors.push_back(imageToBinaryVector(image));
        }
        file.close();
    }
    return binaryVectors;
}




int hammingDistance(const BinaryPoint& p1, const BinaryPoint& p2) {     // Function to compute the Hamming distance between two binary points.
    int distance = 0;
    for (int i = 0; i < (int)p1.size(); i++) {
        if (p1[i] != p2[i]) {
            distance++;
        }
    }
    return distance;
}



struct Hypercube {
    int numBits;                                  // Number of bits in the binary representation.
    int numTables;                               // Number of hash tables.
    int numFunctions;                           // Number of hash functions per table.
    vector<vector<vector<BinaryPoint>>> buckets;        // Buckets for storing binary points.

   
    Hypercube(int numBits, int numTables, int numFunctions) {
        this->numBits = numBits;
        this->numTables = numTables;
        this->numFunctions = numFunctions;


        buckets.resize(numTables);              // Resize the buckets vector to the number of tables.
        for (int i = 0; i < numTables; i++) {
            buckets[i].resize(pow(2, numBits));
        }
    }

    
    void addPoint(const BinaryPoint& point) {       // Function to add a binary point to the hypercube.
        for (int i = 0; i < numTables; i++) {
            int hashValue = hashFunction(point, i);
            buckets[i][hashValue].push_back(point);
        }
    }

    BinaryPoint findNearestNeighbor(const BinaryPoint& query) {     // Function to find the nearest neighbor to a query point in the hypercube.
        int minDistance = numeric_limits<int>::max();               // Initialize the minimum distance to the maximum integer value.
        BinaryPoint nearestNeighbor;

        for (int i = 0; i < numTables; i++) {
            int hashValue = hashFunction(query, i);                 // Compute the hash value for the query point.
            for (const BinaryPoint& point : buckets[i][hashValue]) {
                int distance = hammingDistance(query, point);
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestNeighbor = point;
                }
            }
        }

        return nearestNeighbor;
    }

  
    int hashFunction(const BinaryPoint& point, int tableIndex) {        // Function to compute the hash value for a binary point.
        int hashValue = 0;
        for (int i = 0; i < numFunctions; i++) {
            int bitIndex = (tableIndex * numFunctions) + i;         
            hashValue += point[bitIndex % point.size()];
        }
        return hashValue % buckets[tableIndex].size();
    }

};



 void printHypercube(const Hypercube& hypercube) {                  // Function to print the contents of the hypercube.
    for (int i = 0; i < hypercube.numTables; i++) {                 
        cout << "Table " << i << ":" << endl;                       
        for (int j = 0; j < (int)hypercube.buckets[i].size(); j++) {         
            cout << "Bucket " << j << ": ";
            for (const BinaryPoint& point : hypercube.buckets[i][j]) {
                for (int bit : point) {
                    cout << bit;
                }
                cout << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}



BinaryPoint findNearestNeighbor(const BinaryPoint& query, const vector<BinaryPoint>& data) {    // Function to find the nearest neighbor to a query point in a set of binary points.
    int minDistance = numeric_limits<int>::max();             // Initialize the minimum distance to the maximum integer value.
    BinaryPoint nearestNeighbor;

    for (const BinaryPoint& point : data) {
        int distance = hammingDistance(query, point);
        if (distance < minDistance) {
            minDistance = distance;
            nearestNeighbor = point;
        }
    }

    return nearestNeighbor;
}



vector<BinaryPoint> findKNearestNeighbors(const BinaryPoint& query, const vector<BinaryPoint>& data, int k) {   // Function to find the k-nearest neighbors to a query point in a set of binary points.
    vector<pair<BinaryPoint, int>> distances;

    for (const BinaryPoint& point : data) {                 // Compute the Hamming distance between the query point and each data point.
        int distance = hammingDistance(query, point);
        distances.push_back(make_pair(point, distance));
    }

    sort(distances.begin(), distances.end(), [](const auto& lhs, const auto& rhs) {    // Sort the data points by their Hamming distance from the query point.
        return lhs.second < rhs.second;
    });

    vector<BinaryPoint> kNearestNeighbors;
    for (int i = 0; i < min(k, static_cast<int>(distances.size())); i++) {
        kNearestNeighbors.push_back(distances[i].first);                // Add the k-nearest neighbors to the vector.
    }

    return kNearestNeighbors;
}


vector<BinaryPoint> rangeSearch(const BinaryPoint& query, const vector<BinaryPoint>& data, int radius) {    // Function to find all points within a given radius of a query point in a set of binary points.
    vector<BinaryPoint> withinRadius;

    for (const BinaryPoint& point : data) {             
        int distance = hammingDistance(query, point);           // Compute the Hamming distance between the query point and each data point.
        if (distance <= radius) {
            withinRadius.push_back(point);
        }
    }

    return withinRadius;
}



int main() {
    
    // srand(time(nullptr)); 

    string filename = "train-images.idx3-ubyte";

    int numImagesToRead = 10;

    int d = 10;                         // Dimension of the binary hypercube.
    // vector<BinaryPoint> data; 
    vector<BinaryPoint> queries; 



    vector<BinaryPoint> data = readMNISTImages(filename, numImagesToRead);

    // Generate random binary data points.
    // for (int i = 0; i < 100; i++) {
    //     data.push_back(generateRandomBinaryPoint(d));
    // }


    int numBits = 4;                        // Number of bits in the binary representation.
    int numTables = 1;                      // Number of hash tables.
    int numFunctions = 3;                   // Number of hash functions per table.

    Hypercube hypercube(numBits, numTables, numFunctions);

    
    for (const BinaryPoint& point : data) {         // Add each data point to the hypercube.
        hypercube.addPoint(point);
    }

    printHypercube(hypercube);


   
    for (int i = 0; i < 10; i++) {
        queries.push_back(generateRandomBinaryPoint(d));        // Generate 10 random queries.
    }


    BinaryPoint query = queries[0];
    BinaryPoint nearest = findNearestNeighbor(query, data);     // Find the nearest neighbor to the first query point.
    cout << "Nearest Neighbor: ";
    for (int bit : nearest) {
        cout << bit; 
    }
    cout << endl;
    cout << endl;

    int k = 5;                                                      // Find the 5-nearest neighbors to the first query point.
    vector<BinaryPoint> kNearest = findKNearestNeighbors(query, data, k);   
    cout << "K-Nearest Neighbors: ";
    for (const BinaryPoint& point : kNearest) {
        for (int bit : point) {
            cout << bit; 
        }
        cout << endl;
        cout << endl;
    }
    cout << endl;
    cout << endl;

    int radius = 1000;                                                      // Find all points within a radius of 1000 of the first query point.
    vector<BinaryPoint> withinRadius = rangeSearch(query, data, radius);
    cout << "Points within Radius: " << endl;
    for (const BinaryPoint& point : withinRadius) {
        for (int bit : point) {
            cout << bit;
        }
        cout << endl;
        cout << endl;
    }
    cout << endl;
    cout << endl;

    return 0;
}