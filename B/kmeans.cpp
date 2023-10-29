#include <omp.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

class Point
{
private:
    int pointId, clusterId;
    int dimensions;
    vector<double> values;

    vector<double> lineToVec(string &line)
    {
        vector<double> values;
        string tmp = "";

        for (int i = 0; i < (int)line.length(); i++)
        {
            if ((48 <= int(line[i]) && int(line[i])  <= 57) || line[i] == '.' || line[i] == '+' || line[i] == '-' || line[i] == 'e')
            {
                tmp += line[i];
            }
            else if (tmp.length() > 0)
            {
                values.push_back(stod(tmp));
                tmp = "";
            }
        }
        if (tmp.length() > 0)
        {
            values.push_back(stod(tmp));
            tmp = "";
        }

        return values;
    }

public:
    Point(int id, string line)
    {
        pointId = id;
        values = lineToVec(line);
        dimensions = values.size();
        clusterId = 0; // Initially not assigned to any cluster
    }

    int getDimensions() const { return dimensions; }  // Mark as const

    int getCluster() const { return clusterId; }  // Mark as const

    int getID() const { return pointId; }  // Mark as const

    void setCluster(int val) { clusterId = val; }

    double getVal(int pos) const { return values[pos]; }  // Mark as const
};

class Cluster
{
private:
    int clusterId;
    vector<double> centroid;
    vector<Point> points;

public:
    Cluster(int clusterId, Point centroid)
    {
        this->clusterId = clusterId;
        for (int i = 0; i < centroid.getDimensions(); i++)
        {
            this->centroid.push_back(centroid.getVal(i));
        }
        this->addPoint(centroid);
    }

    void addPoint(Point p)
    {
        p.setCluster(this->clusterId);
        points.push_back(p);
    }

    bool removePoint(int pointId)
    {
        int size = points.size();

        for (int i = 0; i < size; i++)
        {
            if (points[i].getID() == pointId)
            {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    void removeAllPoints() { points.clear(); }

    int getId() const { return clusterId; }  // Mark as const

    const Point& getPoint(int pos) const { return points[pos]; }  // Mark as const

    int getSize() const { return points.size(); }  // Mark as const

    double getCentroidByPos(int pos) const { return centroid[pos]; }  // Mark as const

    void setCentroidByPos(int pos, double val) { this->centroid[pos] = val; }
};


double calculateDistance(const Point &point, const Cluster &cluster)
{
    double distance = 0.0;
    int dimensions = point.getDimensions();

    for (int i = 0; i < dimensions; i++)
    {
        double diff = point.getVal(i) - cluster.getCentroidByPos(i);
        distance += diff * diff;
    }

    return sqrt(distance);
}



class KMeansPlusPlus
{
private:
    int K, iters, dimensions, total_points;
    vector<Cluster> clusters;
    string output_dir;

    // New function for K-Means++ initialization
    void initializeClustersKMeansPlusPlus(vector<Point> &all_points)
    {
        // Choose the first centroid randomly
        random_device rd;
        mt19937 gen(rd()); 
        uniform_int_distribution<int> distribution(0, total_points - 1);

        int firstCentroidIdx = distribution(gen);
        Cluster firstCluster(1, all_points[firstCentroidIdx]);
        clusters.push_back(firstCluster);
        all_points[firstCentroidIdx].setCluster(1);

        for (int k = 2; k <= K; k++)
        {
            vector<double> minDistances;
            double totalDistance = 0.0;

            // Calculate the distance of each point to its nearest cluster
            for (int i = 0; i < total_points; i++)
            {
                double minDist = numeric_limits<double>::max();
                for (int j = 0; j < k - 1; j++)
                {
                    double dist = calculateDistance(all_points[i], clusters[j]);
                    if (dist < minDist)
                        minDist = dist;
                }
                minDistances.push_back(minDist);
                totalDistance += minDist;
            }

            // Randomly choose the next centroid based on probability
            uniform_real_distribution<double> probDistribution(0, totalDistance);
            double randValue = probDistribution(gen);

            for (int i = 0; i < total_points; i++)
            {
                randValue -= minDistances[i];
                if (randValue <= 0)
                {
                    Cluster cluster(k, all_points[i]);
                    clusters.push_back(cluster);
                    all_points[i].setCluster(k);
                    break;
                }
            }
        }
    }

    void clearClusters()
    {
        for (int i = 0; i < K; i++)
        {
            clusters[i].removeAllPoints();
        }
    }

    int getNearestClusterId(Point point)
    {
        double sum = 0.0, min_dist;
        int NearestClusterId;
        if(dimensions==1) {
            min_dist = abs(clusters[0].getCentroidByPos(0) - point.getVal(0));
        }	
        else 
        {
            for (int i = 0; i < dimensions; i++)
            {
                sum += pow(clusters[0].getCentroidByPos(i) - point.getVal(i), 2.0);
                // sum += abs(clusters[0].getCentroidByPos(i) - point.getVal(i));
            }
            min_dist = sqrt(sum);
        }
        NearestClusterId = clusters[0].getId();

        for (int i = 1; i < K; i++)
        {
            double dist;
            sum = 0.0;
            
            if(dimensions==1) {
                    dist = abs(clusters[i].getCentroidByPos(0) - point.getVal(0));
            } 
            else {
                for (int j = 0; j < dimensions; j++)
                {
                    sum += pow(clusters[i].getCentroidByPos(j) - point.getVal(j), 2.0);
                    // sum += abs(clusters[i].getCentroidByPos(j) - point.getVal(j));
                }

                dist = sqrt(sum);
                // dist = sum;
            }
            if (dist < min_dist)
            {
                min_dist = dist;
                NearestClusterId = clusters[i].getId();
            }
        }

        return NearestClusterId;
    }


    // Rest of the K-Means code remains the same

public:
    KMeansPlusPlus(int K, int iterations, string output_dir)
    {
        this->K = K;
        this->iters = iterations;
        this->output_dir = output_dir;
    }

        // Add this function inside the KMeansPlusPlus class
    void printClusterAssignments(const vector<Point>& all_points) const
    {
        for (int i = 0; i < K; i++)
        {
            cout << "Cluster " << clusters[i].getId() << " Points: ";
            for (int j = 0; j < clusters[i].getSize(); j++)
            {
                cout << clusters[i].getPoint(j).getID() << " ";
            }
            cout << endl;
        }
        cout << "Cluster Assignments: ";
        for (int i = 0; i < total_points; i++)
        {
            cout << all_points[i].getCluster() << " ";
        }
        cout << endl;
    }


    void updateCentroidsMaqueen()
{
    for (int i = 0; i < K; i++)
    {
        int ClusterSize = clusters[i].getSize();
        
        for (int j = 0; j < dimensions; j++)
        {
            double sum = 0.0;
            if (ClusterSize > 0)
            {
                #pragma omp parallel for reduction(+: sum) num_threads(16)
                for (int p = 0; p < ClusterSize; p++)
                {
                    sum += clusters[i].getPoint(p).getVal(j);
                }
                clusters[i].setCentroidByPos(j, sum / ClusterSize);
            }
        }
    }
}


    void run(vector<Point>& all_points)
{
    total_points = all_points.size();
    dimensions = all_points[0].getDimensions();

    // Initialize clusters using K-Means++
    initializeClustersKMeansPlusPlus(all_points);

    cout << "Clusters initialized = " << clusters.size() << endl << endl;
    cout << "Running K-Means Clustering.." << endl;

    int iter = 1;
    while (true)

    {

        cout<<"==========================\n";
        cout << "Iter -> " << iter << "/" << iters << endl;
        
        printClusterAssignments(all_points); // Add this line to print cluster assignments
        bool done = true;

        // Assign data points to nearest clusters (K-Means assignment step)
        #pragma omp parallel for reduction(&&: done) num_threads(16)
        for (int i = 0; i < total_points; i++)
        {
            int currentClusterId = all_points[i].getCluster();
            int nearestClusterId = getNearestClusterId(all_points[i]);

            if (currentClusterId != nearestClusterId)
            {
                all_points[i].setCluster(nearestClusterId);
                done = false;
            }
        }
        
        // printClusterAssignments(all_points); // Add this line to print cluster assignments
        // Clear existing clusters
        clearClusters();

        
        // Reassign points to clusters
        
        for (int i = 0; i < total_points; i++)
        {
            int nearestClusterId = getNearestClusterId(all_points[i]);
            all_points[i].setCluster(nearestClusterId);
        }


        // Update centroids using Maqueen method
        updateCentroidsMaqueen();


        cout<<" maqueen update here \n"<<endl;
        // printClusterAssignments(all_points); // Add this line to print cluster assignments

        if (done || iter >= iters)
        {
            cout << "Clustering completed in iteration : " << iter << endl << endl;
            break;
        }
        iter++;
    }

     ofstream pointsFile;   // Write cluster assignments to file
        pointsFile.open(output_dir + "/" + to_string(K) + "-points.txt", ios::out);

        for (int i = 0; i < total_points; i++)
        {
            pointsFile << all_points[i].getCluster() << endl;
        }

        pointsFile.close();

        // Write cluster centers to file
        ofstream outfile;
        outfile.open(output_dir + "/" + to_string(K) + "-clusters.txt");
        if (outfile.is_open())
        {
            for (int i = 0; i < K; i++)
            {
                cout << "Cluster " << clusters[i].getId() << " centroid : ";
                for (int j = 0; j < dimensions; j++)
                {
                    cout << clusters[i].getCentroidByPos(j) << " ";    // Output to console
                    outfile << clusters[i].getCentroidByPos(j) << " "; // Output to file
                }
                cout << endl;
                outfile << endl;
            }
            outfile.close();
        }
        else
        {
            cout << "Error: Unable to write to clusters.txt";
        }
        

        // Rest of the K-Means code remains the same
    }
};






int main(int argc, char **argv)
{
    // Need 3 arguments (except filename) to run, else exit
    if (argc != 4)
    {
        cout << "Error: command-line argument count mismatch. \n ./kmeans <INPUT> <K> <OUT-DIR>" << endl;
        return 1;
    }

    string output_dir = argv[3];

    // Fetching number of clusters
    int K = atoi(argv[2]);

    // Open file for fetching points
    string filename = argv[1];
    ifstream infile(filename.c_str());

    if (!infile.is_open())
    {
        cout << "Error: Failed to open file." << endl;
        return 1;
    }

    // Fetching points from file
    int pointId = 1;
    vector<Point> all_points;
    string line;

    while (getline(infile, line))
    {
        Point point(pointId, line);
        all_points.push_back(point);
        pointId++;
    }
    
    infile.close();
    cout << "\nData fetched successfully!" << endl
         << endl;

    // Return if number of clusters > number of points
    if ((int)all_points.size() < K)
    {
        cout << "Error: Number of clusters greater than number of points." << endl;
        return 1;
    }

    // Running K-Means Clustering
    int iters = 100;

    KMeansPlusPlus kmeans(K, iters, output_dir);
    kmeans.run(all_points);

    return 0;
}