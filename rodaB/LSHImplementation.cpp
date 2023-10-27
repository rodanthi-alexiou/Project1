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



double normal_distribution_generator(void){

	/* Based on https://en.cppreference.com/w/cpp/numeric/random/normal_distribution */

	random_device rd{};
    mt19937 gen{rd()};

	normal_distribution<float> d{0,1};

	map<int, int> hist{};

	return round(d(gen));

}

// long long int modulo(long long int a, long long int b); /* Modulo between a and b, supports negative numers */

long long int modulo(long long int a, long long int b){

    /* Modulo between a and b, supports negative numers */

	return (a % b + b) %b;

}




// long long unsigned int euclidian_hash_query(unsigned char* query_line, int amp_func, int k, fileData& qdata, const std::vector<double>& t); /* Returns the position in the hash table of the query vector */

long long unsigned int euclidian_hash_query(int query_line,int amp_func,int k, fileData& qdata, const std::vector<double>& t){
	
	string string_to_hash;
	long long int sum;
	double x;
	long long unsigned int pos;

		string_to_hash = "";

		for(int h = 0 ; h < k ; h++ ){ /* We have to hash each vector with each Hash Function in the amp_func Amplified Function */

			sum = 0;

			for(int i = 0 ; i < qdata.image_size ; i++ ){ /* In order to calculate the Inner product of the hash vecor and the data vector we have to multiply all of their coordinates */

                x = (double)qdata.images[query_line][i]; /* Get the first int from the file */
                sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
			}


			sum = sum + t[h];
            // cout << t[h] << endl;
			sum = floor( sum / (double) w );
			string_to_hash =  string_to_hash + to_string(sum);

		}

        int n = qdata.number_of_images;
		pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
		pos = modulo(pos,n/2);

		// cout << "Pushing " << line << " to [" << amp_func << "][" << pos << "]" << endl; 

		return pos;


}


// long long int calcute_euclidian_distance(int input_line, int query_line, fileData& fdata, fileData& qdata); /* Returns the euclidian distance between two vectors */

long long int calcute_euclidian_distance(int input_line, int query_line, fileData& fdata, fileData& qdata){

	/* Now lets calcute the inner product of the two vectors and the norm of each one*/

	double x,y;

	long long int dist=0;

	for(int i=0;i<fdata.image_size;i++){

		x = (double)fdata.images[input_line][i];
        y = (double)qdata.images[query_line][i];
		dist = dist + (x-y)*(x-y);
		// cout << "x: " << x << " y: " << y << "dist: " << dist << endl;

	}

	// cout << "DIST: " << dist << endl;

	return dist;
}

void Euclidian_put_hash(int line, int L, int k, fileData& data, const std::vector<double>& t); /* Puts a vector to all of the hash tables */

void Euclidian_put_hash(int index, int L, int k, fileData& data, const std::vector<double>& t){

        int n = data.number_of_images;
        int dim = data.image_size;
        string string_to_hash;
        long long int sum;
        double x;
        string str;
        long long unsigned int pos;

        for (int amp_func = 0; amp_func < L; amp_func++){
            string_to_hash = "";


            for(int h = 0 ; h < k ; h++ ){ /* We have to hash each vector with each Hash Function in the amp_func Amplified Function */
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
		    //cout << "Pushing " << index << " to [" << amp_func << "][" << pos << "]" << endl; 
		    Euclidian_Hash_Tables[amp_func][pos].push_back(index);


        }
} /* Puts a vector to all of the hash tables */




unsigned char** read_mnist_images(std::ifstream& file, fileData& data) {
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

unsigned char* getImageLine(const fileData& data, int imageIndex) {
    if (imageIndex >= 0 && imageIndex < data.number_of_images) {
        return data.images[imageIndex];
    } else {

        return nullptr;
    }
}


//////////////////////////////////////////


// Function to initialize vectors and hash functions
void initializeVectors(int k, int dim) {
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

// Function to initialize Amplified Hash Functions
void initializeAmplifiedFunctions(int L, int k) {
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

// Function to initialize t for each hash function
void initializeT(int k, int w) {
    t.clear();
    uniform_real_distribution<float> distribution(0.0, static_cast<float>(w));

    default_random_engine generator;

    for (int i = 0; i < k; i++) {
        float random = distribution(generator);
        t.push_back(random);
    }
}

void cleanupHashTables(int L, fileData& data, int n) {
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

void initializeLSH(int L, int n, int k, int dim, int w) {
    // Initialize the Hash Tables
    Euclidian_Hash_Tables.resize(L, vector<vector<long long unsigned int>>(n / 2, vector<long long unsigned int>(1000)));

    // Initialize vectors and hash functions
    initializeVectors(k, dim);

    // Initialize Amplified Hash Functions
    initializeAmplifiedFunctions(L, k);

    // Initialize t for each hash function
    initializeT(k, w);
}


int FindNearestNeighbor(int query_line, int L, int k,  fileData& data,  fileData& qData,  std::vector<double>& t) {
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