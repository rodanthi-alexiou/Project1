#include   <iostream>
#include   <fstream>
#include   <string>
#include   <stdexcept>
#include   <vector>
#include   <cmath>
#include   <random>
#include   <chrono>
#include   <algorithm>
#include   <unordered_map>
#include   <unordered_set>
#include   <queue>
#include   <climits>
#include   <cassert>




using namespace std;

ifstream file; /* Input File */

struct fileData {
	int magic_number;
	int number_of_images;
	int n_rows;
	int n_cols;
	int image_size;
	unsigned char** images;
};


unsigned char** read_mnist_images(std::ifstream& file) {
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;

    //ifstream file(full_path, ios::binary);

    // inFile.open(full_path, ios::binary);

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

// get number of images
int get_mnist_lines(ifstream& file) {
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;



    if(file.is_open()) {
        int magic_number = 0, n_rows = 0, n_cols = 0, number_of_images = 0;


                file.read((char *)&magic_number, sizeof(magic_number));
                magic_number = reverseInt(magic_number);
                if(magic_number != 2051) throw runtime_error("Invalid MNIST image file!");

        file.read((char *)&number_of_images, sizeof(number_of_images)), number_of_images = reverseInt(number_of_images);


    return number_of_images;
    } else {
        throw runtime_error("Cannot open file!");
    }
}



int main(int argc, char *argv[]) {

    string str;

    		cout << "Please specify an input file: ";

		cin >> str;

		// inFile.open(str);

		std::ifstream file("train-images.idx3-ubyte", std::ios::binary);


    unsigned char** images = read_mnist_images(file);

    for(int i = 0; i < image_size; i++) {
        cout << (int)images[0][i] << " ";
    }
    cout << endl;

    
    cout << number_of_images << endl;
    return 0;
}