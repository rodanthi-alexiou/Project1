#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

// Define the dimension of the vectors for MNIST images
#define DIMENSION 784

// Define the number of hash functions, buckets, and vectors
#define NUM_HASH_FUNCTIONS 5
#define NUM_BUCKETS 1000
#define NUM_VECTORS 20

// Define a vector structure
typedef struct {
    double data[DIMENSION];
} Vector;

// Define a hash table structure
typedef struct {
    Vector *vectors[NUM_BUCKETS];
    int count[NUM_BUCKETS];
} HashTable;

// Function to calculate the Euclidean distance between two vectors
double euclideanDistance(Vector v1, Vector v2) {
    double distance = 0.0;
    for (int i = 0; i < DIMENSION; i++) {
        double diff = v1.data[i] - v2.data[i];
        distance += diff * diff;
    }
    return sqrt(distance);
}

// Function to generate random projection vectors
Vector generateRandomProjectionVector() {
    Vector randomVector;
    for (int i = 0; i < DIMENSION; i++) {
        randomVector.data[i] = ((double)rand() / RAND_MAX) * 2 - 1; // Random number between -1 and 1
    }
    return randomVector;
}

// Function to compute the hash value for a vector using random projection
int computeHash(Vector v, Vector projectionVector) {
    double dotProduct = 0.0;
    for (int i = 0; i < DIMENSION; i++) {
        dotProduct += v.data[i] * projectionVector.data[i];
    }
    return (int)(dotProduct >= 0);
}

// Function to insert a vector into the hash table
void insertIntoHashTable(HashTable *hashTable, Vector v) {
    int hashValue = 0;
    for (int i = 0; i < NUM_HASH_FUNCTIONS; i++) {
        Vector projectionVector = generateRandomProjectionVector();
        hashValue = (hashValue << 1) | computeHash(v, projectionVector);
    }
    int bucketIndex = hashValue % NUM_BUCKETS;
    hashTable->vectors[bucketIndex] = &v;
    hashTable->count[bucketIndex]++;
}

// Function to find the nearest neighbor to a query vector q
Vector* findNearestNeighbor(HashTable *hashTable, Vector q) {
    Vector *nearestNeighbor = NULL;
    double nearestDistance = DBL_MAX; // Initialize with a very large value

    for (int i = 0; i < NUM_BUCKETS; i++) {
        if (hashTable->count[i] > 0) {
            Vector *v = hashTable->vectors[i];
            double distance = euclideanDistance(*v, q);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestNeighbor = v;
            }
        }
    }

    return nearestNeighbor;
}

int main() {
    // Seed the random number generator with the current time
    srand(time(NULL));

    // Initialize the hash table
    HashTable hashTable;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        hashTable.count[i] = 0;
    }

    // Read MNIST image vectors and insert them into the hash table
    FILE *file = fopen("query.dat", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Skip the header and go to the start of the pixel data
    fseek(file, 16, SEEK_SET);

    Vector imageVector;
    for (int i = 0; i < NUM_VECTORS; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            fread(&(imageVector.data[j]), sizeof(double), 1, file);
        }
        insertIntoHashTable(&hashTable, imageVector);
    }

    // Close the file
    fclose(file);

    // Initialize a query vector q (use MNIST image at index 0)
    Vector q;
    for (int i = 0; i < DIMENSION; i++) {
        q.data[i] = 128.0;  // Set all values to 128 (within the range [0, 256])
    }


    // Find the nearest neighbor to the query vector
    Vector *nearestNeighbor = findNearestNeighbor(&hashTable, q);

    // Print the query vector
    printf("Query Vector q: [");
    for (int j = 0; j < DIMENSION - 1; j++) {
        printf("%lf, ", q.data[j]);
    }
    printf("%lf]\n", q.data[DIMENSION - 1]);

    // Print the nearest neighbor
    if (nearestNeighbor != NULL) {
        printf("Nearest Neighbor to q: [");
        for (int j = 0; j < DIMENSION - 1; j++) {
            printf("%lf, ", nearestNeighbor->data[j]);
        }
        printf("%lf]\n", nearestNeighbor->data[DIMENSION - 1]);
    }

    return 0;
}
