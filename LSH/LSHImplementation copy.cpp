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

std::vector<std::vector<int>> LSHImplementation::Euclidian_Amplified_Functions;
std::vector<std::vector<std::vector<long long unsigned int>>> LSHImplementation::Euclidian_Hash_Tables;

std::vector<std::vector<double>> LSHImplementation::v;
std::vector<double> LSHImplementation::t;
int LSHImplementation::w;
int LSHImplementation::k;
int LSHImplementation::L;

const std::vector<std::vector<std::vector<long long unsigned int>>>& LSHImplementation::getEuclidianHashTables() const {
    return Euclidian_Hash_Tables;
}
const std::vector<std::vector<int>>& LSHImplementation::getEuclidianAmplifiedFunctions() {
    return Euclidian_Amplified_Functions;
}

int LSHImplementation::getAmplifiedFunction(int j, int k) {
    return Euclidian_Amplified_Functions[j][k];
}

LSHImplementation::LSHImplementation(int L, int k, int w) {
    this->L = L;
    this->k = k;
    this->w = w;
}

int LSHImplementation::getL() {
    return L;
}

int LSHImplementation::getK() {
    return k;
}

int LSHImplementation::getW() {
    return w;
}

// Συνάρτηση για παραγωγή τυχαίων αριθμών από κανονική κατανομή
double LSHImplementation::normal_distribution_generator(void){
    random_device rd{};
    mt19937 gen{rd()};
    normal_distribution<float> d{0,1};
    map<int, int> hist{};
    return round(d(gen));
}

// Συνάρτηση για υπολογισμό του υπολοίπου
long long int LSHImplementation::modulo(long long int a, long long int b){
    return (a % b + b) % b;
}

// Συνάρτηση για το hash των διανυσμάτων της διάνυσμα των ερωτήσεων
long long unsigned int LSHImplementation::euclidian_hash_query(int query_line,int amp_func,int k, fileData& qdata, const std::vector<double>& t){
    string string_to_hash;
    long long int sum;
    double x;
    long long unsigned int pos;

    string_to_hash = "";

    for(int h = 0 ; h < k ; h++ ){ // Πρέπει να κατανείμουμε το κάθε διάνυσμα με κάθε Hash Function στις amp_func αναπαραστάσεις του
    
        sum = 0;

        for(int i = 0 ; i < qdata.image_size ; i++ ){ // Για να υπολογίσουμε το εσωτερικό γινόμενο του διανύσματος hash με το διάνυσμα δεδομένων πρέπει να πολλαπλασιάσουμε όλες τις συντεταγμένες τους
            x = (double)qdata.images[query_line][i]; // Πάρε το πρώτο int από το αρχείο
            sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
        }

        sum = sum + t[h];
        sum = floor( sum / (double) w );
        string_to_hash =  string_to_hash + to_string(sum);
    }

    int n = qdata.number_of_images;
    pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
    pos = modulo(pos,n/2);

    return pos;
}

// Συνάρτηση για το hash των κεντροειδών (για Clustering)
long long unsigned int LSHImplementation::euclidian_hash_centroid(unsigned char* query_line, int amp_func, int k, fileData& data,  vector<double>& t) {
    string string_to_hash;
    long long int sum;
    double x;
    long long unsigned int pos;

    string_to_hash = "";

    for (int h = 0; h < k; h++) {
        sum = 0;

        for (int i = 0; i < data.image_size; i++) {
            x = (double)query_line[i];
            sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
        }

        sum = sum + t[h];
        sum = floor(sum / (double)w);
        string_to_hash = string_to_hash + to_string(sum);
    }

    int n = data.number_of_images;
    pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
    pos = modulo(pos, n / 2);

    return pos;
}

// Συνάρτηση για υπολογισμό της ευκλείδιας απόστασης
long long int LSHImplementation::calcute_euclidian_distance(int input_line, int query_line, fileData& fdata, fileData& qdata){
    double x, y;
    long long int dist = 0;
    for(int i=0; i < fdata.image_size; i++){
        x = (double)fdata.images[input_line][i];
        y = (double)qdata.images[query_line][i];
        dist = dist + (x - y) * (x - y);
    }
    return dist;
}

// Συνάρτηση για τον τοποθέτηση ενός διανύσματος σε όλους τους πίνακες hash
void LSHImplementation::Euclidian_put_hash(int index, int L, int k, fileData& data, const std::vector<double>& t){
    int n = data.number_of_images;
    int dim = data.image_size;
    string string_to_hash;
    long long int sum;
    double x;
    string str;
    long long unsigned int pos;

    for (int amp_func = 0; amp_func < L; amp_func++){
        string_to_hash = "";
        for(int h = 0 ; h < k ; h++ ){ 
            sum = 0;
            for(int i=0; i < dim; i++){
                x = (double) data.images[index][i];
                sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
            }
            sum = sum + t[h];
            sum = floor( sum / (double) w );
            string_to_hash.append(to_string(sum));
        }
        pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
        pos = modulo(pos,n/2);
        Euclidian_Hash_Tables[amp_func][pos].push_back(index);
    }
}

// Συνάρτηση για τον τοποθέτηση ενός κεντροειδούς σε όλους τους πίνακες hash
void LSHImplementation::insertCentroidIntoLSH(int index, int number_centroids, int L, int k, const std::vector<double>& t, const std::vector<unsigned char*>& centroid) {
    int n = number_centroids;
    int dim = centroid.size();
    string string_to_hash;
    long long int sum;
    double x;
    string str;
    long long unsigned int pos;

    for (int amp_func = 0; amp_func < L; amp_func++) {
        string_to_hash = "";

        for (int h = 0; h < k; h++) {
            sum = 0;

            for (int i = 0; i < dim; i++) {
                x = (double)centroid[index][i];
                sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
            }

            sum = sum + t[h];
            sum = floor(sum / (double)w);
            string_to_hash.append(to_string(sum));
        }

        pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
        pos = modulo(pos, n / 2);
        Euclidian_Hash_Tables[amp_func][pos].push_back(index);
    }
}

// Συνάρτηση για ανάγνωση εικόνων MNIST
unsigned char** LSHImplementation::read_mnist_images(std::ifstream& file, fileData& data) {
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
        data.images = new unsigned char*[data.number_of_images];

        uchar** _dataset = new uchar*[number_of_images];
        for(int i = 0; i < number_of_images; i++) {
            _dataset[i] = new uchar[image_size];
            file.read((char *)_dataset[i], image_size);
        }

        for (int i = 0; i < data.number_of_images; i++) {
            data.images[i] = new unsigned char[data.image_size];
            std::memcpy(data.images[i], _dataset[i], data.image_size);
        }

        return _dataset;
    } else {
        throw runtime_error("Cannot open file!");
    }
}

// Συνάρτηση για ανάγνωση γραμμής εικόνας από τα δεδομένα
unsigned char* LSHImplementation::getImageLine(const fileData& data, int imageIndex) {
    if (imageIndex >= 0 && imageIndex < data.number_of_images) {
        return data.images[imageIndex];
    } else {
        return nullptr;
    }
}

// Συνάρτηση για την αρχικοποίηση των διανυσμάτων και των συναρτήσεων hash
void LSHImplementation::initializeVectors(int k, int dim) {
    v.resize(k, vector<double>(dim));

    default_random_engine generator;
    normal_distribution<double> normal_distribution_generator(0.0, 1.0);

    for (int i = 0; i < k; i++) {
        v[i].clear();
        for (int j = 0; j < dim; j++) {
            v[i].push_back(normal_distribution_generator(generator));
        }
    }
}

// Συνάρτηση για την αρχικοποίηση των αναπαραστάσεων των hash
void LSHImplementation::initializeAmplifiedFunctions(int L, int k) {
    Euclidian_Amplified_Functions.resize(L, vector<int>(k));

    srand(time(0));

    for (int i = 0; i < L; i++) {
        int j = 0;

        Euclidian_Amplified_Functions[i].clear();

        do {
            int func_num = rand() % k;
            if (find(Euclidian_Amplified_Functions[i].begin(), Euclidian_Amplified_Functions[i].end(), func_num) == Euclidian_Amplified_Functions[i].end()) {
                j++;
                Euclidian_Amplified_Functions[i].push_back(func_num);
            }
        } while (j < k);
    }
}

// Συνάρτηση για την αρχικοποίηση του t για κάθε συνάρτηση
void LSHImplementation::initializeT(int k, int w) {
    t.clear();
    uniform_real_distribution<float> distribution(0.0, static_cast<float>(w));

    default_random_engine generator;

    for (int i = 0; i < k; i++) {
        float random = distribution(generator);
        t.push_back(random);
    }
}


// Καθαρίζουμε τους πίνακες hash από τα 0
void LSHImplementation::cleanupHashTables(int L, fileData& data, int n) {
    int max = n / 2;

    for (int amp_func = 0; amp_func < L; amp_func++) {
        for (int pos = 0; pos < max; pos++) {
            Euclidian_Hash_Tables[amp_func][pos].erase(
                std::remove(Euclidian_Hash_Tables[amp_func][pos].begin(), Euclidian_Hash_Tables[amp_func][pos].end(), 0),
                Euclidian_Hash_Tables[amp_func][pos].end()
            );
        }
    }

    Euclidian_put_hash(0, L, k, data, t);
}

void LSHImplementation::initializeLSH(int L, int n, int k, int dim, int w) {

    Euclidian_Hash_Tables.resize(L, vector<vector<long long unsigned int>>(n / 2, vector<long long unsigned int>(1000)));
    initializeVectors(k, dim);
    initializeAmplifiedFunctions(L, k);
    initializeT(k, w);
}


int LSHImplementation::FindNearestNeighbor(int query_line, int L, int k,  fileData& data,  fileData& qData,  std::vector<double>& t) {
    int amp_func, pos_in_hash_table;
    long long unsigned int dist, min_dist = 2147483646;
    short int time_to_break = 3 * L;
    long long unsigned int min_line = 0;

    for (amp_func = 0; amp_func < L; ++amp_func) {
        pos_in_hash_table = euclidian_hash_query(query_line, amp_func, k, qData, t);

        for (const auto& dataPoint : Euclidian_Hash_Tables[amp_func][pos_in_hash_table]) {
            dist = calcute_euclidian_distance(dataPoint, query_line, data, qData);

            if (dist < min_dist) {
                min_line = dataPoint;
                min_dist = dist;
            }

            time_to_break--;

            if (time_to_break <= 0)
                break;
        }

        time_to_break = 3 * L;
    }

    return min_line;
}

int LSHImplementation::FindNearestNeighborCentroid(unsigned char* query_line, int L, int k, fileData& data, std::vector<double>& t) {
    int amp_func, pos_in_hash_table;
    long long unsigned int dist, min_dist = 2147483646;
    short int time_to_break = 3 * L;
    long long unsigned int min_line = 0;

    for (amp_func = 0; amp_func < L; ++amp_func) {
        pos_in_hash_table = euclidian_hash_centroid((unsigned char*)query_line, amp_func, k, data, t);

        for (const auto& dataPoint : Euclidian_Hash_Tables[amp_func][pos_in_hash_table]) {
            long long unsigned int dist = 0;
            for (int i = 0; i < data.image_size; i++) {
                dist += (long long unsigned int)query_line[i] * (long long unsigned int)data.images[dataPoint][i];
            }

            if (dist < min_dist) {
                min_line = dataPoint;
                min_dist = dist;
            }

            time_to_break--;

            if (time_to_break <= 0)
                break;
        }

        time_to_break = 3 * L;
    }

    return min_line;
}

void LSHImplementation::Euclidian_Full_Search_NN(int query_line, fileData& data, fileData& qdata){

	int n = data.number_of_images;
	long long unsigned int min_line=0,min_dist=BIG_INT ,dist;

	for(long long unsigned int i=0;i< n;i++){

		dist = LSHImplementation::calcute_euclidian_distance(i,query_line, data, qdata);

		if(dist < min_dist){
			min_line = i;
			min_dist = dist;
		}

	}

	cout << "Nearest Neighbour: Item " << min_line << endl;
	cout << "distanceTrue: " << min_dist << endl;

}

void LSHImplementation::Euclidian_Full_Search_Range(int query_line, fileData& data, fileData& qdata, double radius){

	int n = data.number_of_images;
	cout << "R-Near Neighbours:" << endl;
	long long unsigned int dist;

	for(long long unsigned int i=0;i< n;i++){

		dist = LSHImplementation::calcute_euclidian_distance(i,query_line, data, qdata);

		if(dist < radius){
			cout << "Item " << i << endl;
		}

	}

}
