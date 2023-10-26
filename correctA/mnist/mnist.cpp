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



using namespace std;

ifstream file; /* Input File */

ifstream qFile; /* Query File */

//static vector<vector<vector<long long unsigned int>>> Euclidian_Hash_Tables; /* Hash Tables */

static vector<vector<vector<long long unsigned int>>> Euclidian_Hash_Tables; /* Hash Tables */

static vector<vector<double> > v; /* Vectors V used in Hash Functions */

static vector< vector<int> > Euclidian_Amplified_Functions; /* Amplified Hash Functions */

static vector<double> t;

static int w;


struct fileData {
	int magic_number;
	int number_of_images;
	int n_rows;
	int n_cols;
	int image_size;
	unsigned char** images; 
};

int k=4; /* Number of LSH Functions */

int L=5; /* Number of Hash Tables  */

double normal_distribution_generator(void); /* Returns a random number following normal distribution */

double normal_distribution_generator(void){

	/* Based on https://en.cppreference.com/w/cpp/numeric/random/normal_distribution */

	random_device rd{};
    mt19937 gen{rd()};

	normal_distribution<float> d{0,1};

	map<int, int> hist{};

	return round(d(gen));

}

long long int modulo(long long int a, long long int b); /* Modulo between a and b, supports negative numers */

long long int modulo(long long int a, long long int b){

    /* Modulo between a and b, supports negative numers */

	return (a % b + b) %b;

}




long long unsigned int euclidian_hash_query(unsigned char* query_line, int amp_func, int k, fileData& qdata); /* Returns the position in the hash table of the query vector */

long long unsigned int euclidian_hash_query(int query_line,int amp_func,int k, fileData& qdata){
	
	string string_to_hash;
	long long int sum;
	double x;
	long long unsigned int pos;

		string_to_hash = "";

		for(int h = 0 ; h < k ; h++ ){ /* We have to hash each vector with each Hash Function in the amp_func Amplified Function */

			sum = 0;

			for(int i = 0 ; i < qdata.image_size ; i++ ){ /* In order to calculate the Inner product of the hash vecor and the data vector we have to multiply all of their coordinates */

                x = (double) qdata.images[query_line][i]; /* Get the first int from the file */
                sum += x * v[Euclidian_Amplified_Functions[amp_func][h]][i];
			}

            cout << "sum: " << sum << endl;

			sum = sum + t[h];
            cout << t[h] << endl;
			sum = floor( sum / (double) w );
            cout << "sum2 " << sum << endl;
			string_to_hash =  string_to_hash + to_string(sum);

		}

        int n = qdata.number_of_images;
		pos = modulo(hash<string>{}(string_to_hash), BIG_INT);
		pos = modulo(pos,n/2);

		// cout << "Pushing " << line << " to [" << amp_func << "][" << pos << "]" << endl; 

		return pos;


}


long long int calcute_euclidian_distance(int input_line, int query_line, fileData& fdata, fileData& qdata); /* Returns the euclidian distance between two vectors */

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


int main(int argc, char *argv[]) {

    // string str;

    // 		cout << "Please specify an input file: ";

	// 	cin >> str;



	std::ifstream file("train-images.idx3-ubyte", std::ios::binary);
    
    fileData data = {
        0,     // magic_number
        0,     // number_of_images
        0,     // n_rows
        0,     // n_cols
        0,     // image_size
        nullptr         // images (empty vector)
    };


    // unsigned char** images = read_mnist_images(file, data);

    unsigned char** images = read_mnist_images(file, data);


    cout << "Magic number: " << data.magic_number << endl;
    cout << "Number of images: " << data.number_of_images << endl;
    cout << "Number of rows: " << data.n_rows << endl;
    cout << "Number of cols: " << data.n_cols << endl;
    cout << "Image Size: " << data.image_size << endl;

    cout << "First image: " << endl;
    // for(int i = 0; i < data.n_rows; i++) {
    //     for(int j = 0; j < data.n_cols; j++) {
    //         cout << (int)images[0][i * data.n_cols + j] << " ";
    //     }
    //     cout << endl;
    // }

    // cout << (int)images[0][0] << endl;

    // for( int i=0; i< data.image_size; i++){
    //     cout << (int)images[0][i] << " ";
    // }

    //////////////////////////////////////////

    // Initialize the Hash Tables

    int n = data.number_of_images; 

    //std::vector<std::vector<std::vector<std::vector<long long unsigned int>>>> Euclidian_Hash_Tables;
    Euclidian_Hash_Tables.resize(L,vector <vector <long long unsigned int> >(n/2, vector <long long unsigned int> (1000)));


	cout << "I just resized the Hash tables with size: " << L << " x " << n/2 << endl;


    // Initialize the Vectors v
    // Initialize Hash Functions

    int dim = data.image_size;

    v.resize(k,vector <double> (dim));

    // cout << "I just resized the vectors v with size: " << k << " x " << n << endl;

    cout << "I just resized the Hash Functions with size: " << k << " x " << dim << endl;

    default_random_engine generator;
    normal_distribution<double> normal_distribution_generator(0.0, 1.0);

    for(int i=0;i<k;i++){
        v[i].clear();
        for(int j=0;j < dim;j++){
            v[i].push_back(normal_distribution_generator(generator));
            //  cout << "i: " << i << " j: " << j << " is " << (double) v[i][j] << endl; /* DEBUG - Printing out elements of Hash_Function */
        }
    }

    cout << "I just filled the vectors v with random numbers" << endl;


	int w = 400;

	/* Initialize Amplified_Euclidian_Functions */

	Euclidian_Amplified_Functions.resize( L , vector<int>(k));

	srand(time(0));
	

	int func_num;

	int j;

	for(int i=0; i<L; i++){   // for each hash table

		j=0;

		Euclidian_Amplified_Functions[i].clear();

		do{

			func_num = rand() % k;

			// cout << "func_num is: " << func_num << " i is: " << i << endl;

			if( find(Euclidian_Amplified_Functions[i].begin(), Euclidian_Amplified_Functions[i].end(), func_num ) == Euclidian_Amplified_Functions[i].end()){

				// cout << "i got in here with value: " << func_num << " i is: " << i << endl;
				j++;

				Euclidian_Amplified_Functions[i].push_back(func_num);

			}

		}while(j < k);

	}

	/* Last but not least, lets initialize the t of each hash function */

    std::vector<double> t;

	t.clear();

	float random;

	// random_device generator;
	uniform_real_distribution<float> distribution (0.0, (float) w);

	for(int i=0;i<k;i++){ 	/* Each hash function has its own t */
		random = distribution(generator); /* Generate a new double number */
		t.push_back(random);
	}


    //////////////////////////////////////////////

    for(int index=0; index < data.number_of_images; index++){

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
                    x = (double) images[index][i];
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

    }


    ///////////////////////////////////////////////////////////////


    // Find the nearest neighbor of a query

    // open query file  
    std::ifstream qFile("t10k-images.idx3-ubyte", std::ios::binary);

    // initialize query file data
    fileData qData = {
        0,     // magic_number
        0,     // number_of_images
        0,     // n_rows
        0,     // n_cols
        0,     // image_size
        nullptr // images (empty vector)
    };

    // read query file
    unsigned char** qImages = read_mnist_images(qFile, qData);

    cout << "Magic number: " << qData.magic_number << endl;
    cout << "Number of images: " << qData.number_of_images << endl;
    cout << "Number of rows: " << qData.n_rows << endl;
    cout << "Number of cols: " << qData.n_cols << endl;
    cout << "Image Size: " << qData.image_size << endl;

    // get nearest neighbor of first query image

    // for( int i=0; i< qData.image_size; i++){
    //     cout << (int)qImages[0][i] << " ";
    // }
    // cout << endl;

    // cout << (int)qImages[0][0] << endl;

    //////////////////////////////////////////


    int amp_func,pos_in_hash_table;

	long long unsigned int dist, min_dist = 2147483646;

	short int time_to_break = 3*L;

	long long unsigned int min_line=0;

	auto start1 = std::chrono::high_resolution_clock::now();

    int query_line = 0;


	for( amp_func = 0;amp_func < L; amp_func++ ){ /* We have to look in each hash table for the query vector */

        cout << "amp_func: " << amp_func << endl;

		pos_in_hash_table = euclidian_hash_query(query_line,amp_func,k, qData);

        cout << "pos in hash table: " << pos_in_hash_table << endl;


		// cout << "pos in hash table: " << pos_in_hash_table << endl;

		for (auto i = Euclidian_Hash_Tables[amp_func][pos_in_hash_table].begin(); i != Euclidian_Hash_Tables[amp_func][pos_in_hash_table].end(); ++i){

			dist = calcute_euclidian_distance(*i, query_line, data, qData); /* Calculate euclidian distance between the two vectors */ 

			if( dist < min_dist ){ /* If the distance we found is less, update min_dist and min_line */

				min_line = *i;

				min_dist = dist;
			}

			time_to_break--;

			if( time_to_break <= 0 ) break; 

		}

		time_to_break = 3*L;

	}


    auto finish1 = std::chrono::high_resolution_clock::now();

	// cout << "Nearest Vector of " << query_line << " is: " << min_line << " with distance: " << min_dist << endl;

	cout << "Query " << query_line << endl;
	cout << "NN(lsh): Item " << min_line << endl;
	cout << "distanceLSH: " << min_dist << endl;


    



    


    return 0;
}