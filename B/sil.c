#include <stdio.h>
#include <math.h>

typedef struct {
    double a; // Average distance within the same cluster
    double b; // Average distance to the next best (neighbor) cluster
} Cluster;  

double calculatesilhouette(Cluster cluster) {
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

int main() {
    Cluster cluster;
    cluster.a = 0.5; // Replace with the actual average distance within the same cluster
    cluster.b = 0.8; // Replace with the actual average distance to the next best cluster

    double silhouette = calculatesilhouette(cluster);
    printf("Silhouette for the cluster: %lf\n", silhouette);
    
    return 0;
}