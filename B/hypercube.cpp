#include <iostream>
#include <vector>
#include <cmath>
#include <random>


// Function to generate a random projection vector for 2D space
std::vector<double> generateRandomProjectionVector() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(-1, 1);

    std::vector<double> projectionVector(2);
    for (int i = 0; i < 2; ++i) {
        projectionVector[i] = distribution(gen);
    }
    return projectionVector;
}

// Function to project a vector onto a random projection vector
double projectOntoRandomVector(const std::vector<double>& vector, const std::vector<double>& projectionVector) {
    double dotProduct = 0.0;
    for (int i = 0; i < 2; ++i) {
        dotProduct += vector[i] * projectionVector[i];
    }
    return dotProduct;
}

int main() {
    // Example data with 2D vectors
    std::vector<std::vector<double>> data = {
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    };

    // Example query vector
    std::vector<double> query = {2.5, 3.5};

    // Number of random projections
    int numProjections = 5;

    std::vector<double> projections;

    for (int i = 0; i < numProjections; ++i) {
        // Generate a random projection vector
        std::vector<double> randomProjectionVector = generateRandomProjectionVector();

        // Project the query vector onto the random projection vector
        double projection = projectOntoRandomVector(query, randomProjectionVector);

        projections.push_back(projection);
    }

    // Find the nearest neighbor in the lower-dimensional space
    double minDistance = std::numeric_limits<double>::max();
    int nearestNeighborIndex = -1;

    for (int i = 0; i < data.size(); ++i) {
        double distance = 0;
        for (int j = 0; j < numProjections; ++j) {
            distance += std::pow(projections[j] - projectOntoRandomVector(data[i], generateRandomProjectionVector()), 2);
        }
        distance = std::sqrt(distance);

        if (distance < minDistance) {
            minDistance = distance;
            nearestNeighborIndex = i;
        }
    }

    std::cout << "Query Vector: (" << query[0] << ", " << query[1] << ")\n";
    std::cout << "Nearest Neighbor: (" << data[nearestNeighborIndex][0] << ", " << data[nearestNeighborIndex][1] << ")\n";

    return 0;
}
