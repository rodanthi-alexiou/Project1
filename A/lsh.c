// Υλοποιήστε τον αλγόριθμο LSH για τα διανύσματα στον d-διάστατο χώρο βάσει της ευκλείδειας μετρικής. (L2)
// καθώς και τον αλγόριθμο τυχαίας προβολής στον υπερκύβο για την ίδια μετρική
// Το πρόγραμμα θα υλοποιηθεί έτσι ώστε λαμβάνοντας ως είσοδο διάνυσμα q και ακέραιους N και R 
// να επιστρέψει α) τον πλησιέστερο γείτονα στο q
// β) τους N πλησιέστερους γείτονες στο q
// γ) τα διανύσματα εντός ακτίνας R από το q (αναζήτηση περιοχής, range search)
// ο σχεδιασμός του κώδικα θα πρέπει να επιτρέπει την εύκολη επέκτασή του σε διανυσματικούς χώρους με άλλη μετρική
// πχ p-norm ή διαφορετικούς χώρους


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <time.h>



#define DIMENSION 10 // The dimension of the vectors
#define NUM_HASH_FUNCTIONS 5 // Number of hash functions
#define NUM_BUCKETS 1000 // Number of buckets in the hash table
#define NUM_VECTORS 1000

// Define a vector structure
typedef struct {
    double data[DIMENSION];
} Vector;

// Define a hash table structure
typedef struct {
    Vector *vectors[NUM_BUCKETS];
    int count[NUM_BUCKETS];
} HashTable;

// Define a structure for the priority queue
typedef struct {
    Vector *vector;
    double distance;
} Neighbor;

// Comparison function for the priority queue
int compareNeighbors(const void *a, const void *b) {
    double distA = ((Neighbor *)a)->distance;
    double distB = ((Neighbor *)b)->distance;
    if (distA < distB) return -1;
    if (distA > distB) return 1;
    return 0;
}

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

// Function to perform a range search around query vector q within radius R
void rangeSearch(HashTable *hashTable, Vector q, double R) {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        if (hashTable->count[i] > 0) {
            Vector *v = hashTable->vectors[i];
            double distance = euclideanDistance(*v, q);
            if (distance <= R) {
                printf("Vector found within radius R: [");
                for (int j = 0; j < DIMENSION - 1; j++) {
                    printf("%lf, ", v->data[j]);
                }
                printf("%lf]\n", v->data[DIMENSION - 1]);
            }
        }
    }
}

int main() {
    // Initialize the hash table
    HashTable hashTable;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        hashTable.count[i] = 0;
    }

    // Input query vector q
    Vector q;
    for (int i = 0; i < DIMENSION; i++) {
        // Initialize q with some sample values (e.g., 1.0, 2.0, 3.0, ...)
        q.data[i] = (double)(i + 1);
    }


srand(time(NULL)); // Seed the random number generator with the current time

    Vector vectors[NUM_VECTORS];

    // Generate random vectors and insert them into the hash table
    for (int i = 0; i < NUM_VECTORS; i++) {
        generateRandomProjectionVector(&vectors[i]);
        // Insert vectors into the hash table using insertIntoHashTable function
        insertIntoHashTable(&hashTable, vectors[i]);
    }



/////////////////////////////////////////////////////////////////////////////////////////

    // Initialize variables to keep track of the nearest neighbor and its distance
    Vector *nearestNeighbor = NULL;
    double nearestDistance = DBL_MAX; // Initialize with a very large value

    // Iterate through the hash table to find the nearest neighbor
    for (int i = 0; i < NUM_BUCKETS; i++) {
        if (hashTable.count[i] > 0) {
            Vector *v = hashTable.vectors[i];
            double distance = euclideanDistance(*v, q);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestNeighbor = v;
            }
        }
    }

    // Print the nearest neighbor
    if (nearestNeighbor != NULL) {
        printf("Nearest Neighbor to q: [");
        for (int j = 0; j < DIMENSION - 1; j++) {
            printf("%lf, ", nearestNeighbor->data[j]);
        }
        printf("%lf]\n", nearestNeighbor->data[DIMENSION - 1]);
    }

/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////

    // Perform a range search
    double R = 2.0; // Radius for range search
    rangeSearch(&hashTable, q, R);

    return 0;
}

