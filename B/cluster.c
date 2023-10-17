#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


#define numClusters 10
#define numVariations 3

// Define a structure to hold the command-line arguments
typedef struct {
    char* inputFileName;
    char* configFileName;
    char* outputFileName;
    char* method;
    int completeFlag;
} CommandLineArgs;


// Ορισμός δομής για την αναπαράσταση των συστάδων
typedef struct {
    int size;
    double centroid;
    int imageNumber;
     double a; // Average distance within the same cluster
    double b; // Average distance to the next best (neighbor) cluster
} Cluster;


// Define global variables
char* algorithmName = "K-Means";
Cluster clusters[numClusters];
clock_t start_time, end_time;
double executionTime;



// Function to parse command-line arguments
CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    args.inputFileName = NULL;
    args.configFileName = NULL;
    args.outputFileName = NULL;
    args.method = NULL;
    args.completeFlag = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            args.inputFileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            args.configFileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            args.outputFileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            args.method = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-complete") == 0) {
            args.completeFlag = 1;
        }
    }

    return args;
}








double calculateSilhouette(Cluster cluster) {
    double a = cluster.a;
    double b = cluster.b;

    if (a < b) {
        return 1 - (a / b);
    } else if (a > b) {
        return b / a - 1;
    } else {
        return 0; // a(i) == b(i)
    }
}

int main(int argc, char* argv[]) {
    // Ανάγνωση των αρχείων εισόδου και επεξεργασία




 CommandLineArgs args = parseCommandLine(argc, argv);

    if (args.inputFileName == NULL || args.configFileName == NULL || args.outputFileName == NULL || args.method == NULL) {
        printf("Usage: %s -i <input file> -c <configuration file> -o <output file> -m <method: Classic OR LSH or Hypercube> [-complete]\n", argv[0]);
        return 1;
    }

    // Perform your operations using the parsed arguments
    printf("Input File: %s\n", args.inputFileName);
    printf("Config File: %s\n", args.configFileName);
    printf("Output File: %s\n", args.outputFileName);
    printf("Method: %s\n", args.method);
    if (args.completeFlag) {
        printf("Complete Flag: Enabled\n");
    } else {
        printf("Complete Flag: Disabled\n");
    }



    start_time = clock(); // NEED TO CHANGE POSITION 






    Cluster cluster;
    cluster.a = 0.5; // Replace with the actual average distance within the same cluster
    cluster.b = 0.8; // Replace with the actual average distance to the next best cluster

    double silhouette = calculateSilhouette(cluster);
    printf("Silhouette for the cluster: %lf\n", silhouette);
    



    // Για κάθε παραλλαγή
    for (int variation = 1; variation <= numVariations; variation++) {
        // Υπολογισμός των συστάδων και αποθήκευση τους στον πίνακα clusters

        // Υπολογισμός χρόνου εκτέλεσης

        // Υπολογισμός Silhouette για κάθε συστάδα και αποθήκευση

        // Εγγραφή των αποτελεσμάτων στο αρχείο εξόδου
        FILE *outputFile = fopen("output.conf", "a");
        fprintf(outputFile, "Algorithm: %s\n", algorithmName);
        for (int i = 0; i < numClusters; i++) {
            fprintf(outputFile, "CLUSTER-%d (size: %d, centroid: %lf, imageNumber: %d)\n",
                i, clusters[i].size, clusters[i].centroid, clusters[i].imageNumber);
        }
        fprintf(outputFile, "clustering time: %lf\n", executionTime);
        fprintf(outputFile, "Silhouette: [");
        for (int i = 0; i < numClusters; i++) {
            fprintf(outputFile, "%lf, ", calculateSilhouette(clusters[i]));
        }
        fprintf(outputFile, "]\n");
        fclose(outputFile);
    }

    end_time = clock(); // NEED TO CHANGE POSITION
    executionTime = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    return 0;
}

