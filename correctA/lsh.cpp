#include "modules/UserInputHandling.h"
#include "modules/CosineHashing.h"
#include "modules/EuclidianHashing.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

ifstream inFile; /* Input File */

ifstream qFile; /* Query File */

ofstream outFile; /* Output File */

int k=4; /* Number of LSH Functions */

int L=5; /* Number of Hash Tables  */

bool metric = 0; /* Metric to be used 0: Euclidian 1: Cosine */

// a struct to hold the data of the file
struct fileData {
	int magic_number;
	int number_of_images;
	int n_rows;
	int n_cols;
	int image_size;
	unsigned char** images;
};


int number_of_images;
int image_size; 

unsigned char** read_mnist_images(std::ifstream& file) {
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;

    if(file.is_open()) {
        int magic_number = 0, n_rows = 0, n_cols = 0;


        file.read((char *)&magic_number, sizeof(magic_number));
        magic_number = reverseInt(magic_number);

        cout << magic_number << endl;

        if(magic_number != 2051) throw runtime_error("Invalid MNIST image file!");

        file.read((char *)&number_of_images, sizeof(number_of_images)), number_of_images = reverseInt(number_of_images);
        file.read((char *)&n_rows, sizeof(n_rows)), n_rows = reverseInt(n_rows);
        file.read((char *)&n_cols, sizeof(n_cols)), n_cols = reverseInt(n_cols);

        cout << "Number of rows: " << n_rows << endl;
        cout << "Number of cols: " << n_cols << endl;

        image_size = n_rows * n_cols;

        cout << "Image Size: " << image_size << endl;

        uchar** _dataset = new uchar*[number_of_images];
        for(int i = 0; i < number_of_images; i++) {
            _dataset[i] = new uchar[image_size];
            file.read((char *)_dataset[i], image_size);
        }
        return _dataset;
    } else {
        throw runtime_error("Cannot open file!");
    }
}




int main(int argc, char *argv[])
{

	// int input = user_input_handling(argc,argv); /* Handle User Input */

	// if( input == -1 ){ 							/* Check if user input was OK */

	// 	cout << "The program will now exit." << endl;

	// 	return -1;

	// }

	int mentric = 0;

	/* If the input was OK, proceed to find if a metric is specified in the file */

	if( metric == 0 ){

		cout << "Using Euclidian metric..."<< endl;
	
	}else{

		cout << "Using Cosine Similarity..."<< endl;

	}

	/* Initializion of Hash Functions and Hash table */

	// long long unsigned int lines = get_number_of_lines();


	std::ifstream file("train-images.idx3-ubyte", std::ios::binary);

    unsigned char** images = read_mnist_images(file);

 	cout << "Number of lines: " << number_of_images << endl;

	int lines = number_of_images;

	if( metric == 0 ){

		HashTable_Euclidian_Initialization(L);

		HashFunctions_Euclidian_Initialization(k,L);

		Euclidian_Hash_Tables_Finalization(L);

		cout << "-----------------------------" << endl;

		for(int i=0; i < lines ;i++){

			Euclidian_Hash_from_file(i,L,k);
			
		}


		 Euclidian_LSH_File(L,k);

	}

	if( metric == 1 ){

		HashTable_Cosine_Initialization(L,k);

		HashFunctions_Cosine_Initialization(L,k);

		for(int i=0; i < lines ;i++){

			Cosine_Hash_from_file(i,L,k);

		}

		cout << "-----------------------------" << endl;

		Hash_Tables_Finalization(L,k);

		Cosine_LSH_File(L,k);

	}

 	outFile.close();
	inFile.close();
	qFile.close(); 

	return 0;

}