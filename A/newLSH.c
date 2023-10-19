#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <time.h> 
  
#define NUM_IMAGES 20  
#define IMAGE_ROWS 28  
#define IMAGE_COLS 28  
#define NUM_PIXELS (IMAGE_ROWS * IMAGE_COLS)  
  
// Define the maximum number of hash tables and hash functions  
#define MAX_NUM_TABLES 100  
#define MAX_NUM_FUNCTIONS 10  
  
// Define the Euclidean distance between two vectors  
double euclidean_distance(double *v1, double *v2, int d) {  
    double distance = 0.0;  
    for (int i = 0; i < d; i++) {  
        distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);  
    }  
    return sqrt(distance);  
}  
  
// Define the hash function using random projection  
void random_projection_hash(double *v, int d, double *b, double *r, int k, int *h) {  
    for (int i = 0; i < k; i++) {  
        double dot_product = 0.0;  
        for (int j = 0; j < d; j++) {  
            dot_product += v[j] * r[i * d + j];  
        }  
        h[i] = (dot_product >= b[i]) ? 1 : 0;  
    }  
}  
  
// Define the LSH algorithm using random projection  
void random_projection_lsh(double **vectors, int n, int d, double *b, double *r, int k, int l, int **buckets) {  
    // Initialize the hash tables  
    int num_buckets = pow(2, k);  
    for (int i = 0; i < l; i++) {  
        for (int j = 0; j < num_buckets; j++) {  
            buckets[i][j] = -1;  
        }  
    }  
      
    // Hash the vectors into the buckets  
    for (int i = 0; i < n; i++) {  
        int *hash_values = (int*) malloc(k * sizeof(int));  
        random_projection_hash(vectors[i], d, b, r, k, hash_values);  
        for (int j = 0; j < l; j++) {  
            int bucket_index = 0;  
            for (int h = 0; h < k; h++) {  
                bucket_index += hash_values[h] * pow(2, h);  
            }  
            if (buckets[j][bucket_index] == -1) {  
                buckets[j][bucket_index] = i;  
            } else {  
                double distance1 = euclidean_distance(vectors[i], vectors[buckets[j][bucket_index]], d);  
                double distance2 = euclidean_distance(vectors[i], vectors[i], d);  
                if (distance2 < distance1) {  
                    buckets[j][bucket_index] = i;  
                }  
            }  
        }  
        free(hash_values);  
    }  
}  
  
// Define the main function  
int main() {  
    // Load the vectors from the .dat file  
    FILE *fp = fopen("vectors.dat", "rb");  
    if (fp == NULL) {  
        printf("Error: Could not open the file.\n");  
        return 1;  
    }  
    double **vectors = (double**) malloc(NUM_IMAGES * sizeof(double*));  
    for (int i = 0; i < NUM_IMAGES; i++) {  
        vectors[i] = (double*) malloc(NUM_PIXELS * sizeof(double));  
        fread(vectors[i], sizeof(double), NUM_PIXELS, fp);  
    }  
    fclose(fp);  
  
    // Set the parameters of the LSH algorithm  
    int N = NUM_IMAGES;  
    int R = 2;  
    int k = 10;  
    int l = 5;  

    // Generate the random projection vectors and thresholds
    double b = (double)malloc(k * sizeof(double));
    double r = (double) malloc(k * d * sizeof(double));
    srand(time(NULL));
    for (int i = 0; i < k; i++) {
    b[i] = (double) rand() / RAND_MAX * R;
    for (int j = 0; j < d; j++) {
    r[i * d + j] = (double) rand() / RAND_MAX * 2 - 1;
    }
    }

// Create the hash tables and hash the vectors  
int **buckets = (int**) malloc(l * sizeof(int*));  
for (int i = 0; i < l; i++) {  
    buckets[i] = (int*) malloc(pow(2, k) * sizeof(int));  
}  
random_projection_lsh(vectors, N, NUM_PIXELS, b, r, k, l, buckets);  

// Input the query vector  
double *q = (double*) malloc(NUM_PIXELS * sizeof(double));  
printf("Enter the query vector:\n");  
for (int i = 0; i < NUM_PIXELS; i++) {  
    scanf("%lf", &q[i]);  
}  

// Hash the query vector and find the nearest neighbor  
int *hash_values = (int*) malloc(k * sizeof(int));  
random_projection_hash(q, NUM_PIXELS, b, r, k, hash_values);  
int nearest_neighbor = -1;  
double min_distance = INFINITY;  
for (int i = 0; i < l; i++) {  
    int bucket_index = 0;  
    for (int h = 0; h < k; h++) {  
        bucket_index += hash_values[h] * pow(2, h);  
    }  
    if (buckets[i][bucket_index] != -1) {  
        double distance = euclidean_distance(q, vectors[buckets[i][bucket_index]], NUM_PIXELS);  
        if (distance < min_distance) {  
            nearest_neighbor = buckets[i][bucket_index];  
            min_distance = distance;  
        }  
    }  
    }  
    free(hash_values);  

    // Output the nearest neighbor and its distance to the query vector  
    printf("The nearest neighbor is image %d with distance %lf.\n", nearest_neighbor, min_distance);  

    // Free the memory  
    free(q);  
    for (int i = 0; i < l; i++) {  
        free(buckets[i]);  
    }  
    free(buckets);  
    free(b);  
    free(r);  
    for (int i = 0; i < NUM_IMAGES; i++) {  
        free(vectors[i]);  
    }  
    free(vectors);  

    return 0;  
    }