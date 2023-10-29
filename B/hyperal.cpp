#include <iostream>
#include <stdint.h>
#include <cmath>
#include "hash.h"
#include "pre_hyper.h"
#define WINDOW 2

using namespace std;

double dist(const uint8_t* array1, const uint8_t* array2, size_t d) {
    double sum = 0.0;
    for (size_t i = 0; i < d; i++) {
        double diff = array1[i] - array2[i];
        sum += diff * diff;
    }

    return std::sqrt(sum);
}

int read_query(const char* filename, uint8_t image[784], int image_index) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 8, SEEK_SET);
    uint32_t magic_number;
    fread(&magic_number, sizeof(uint32_t), 1, file);
    uint32_t total_images;
    fread(&total_images, sizeof(uint32_t), 1, file);

    // Read the pixel data for the image
    fseek(file, 16 + (image_index * 784), SEEK_SET);// Seek to the start of the specified image in the file
    fread(image, sizeof(uint8_t), 784, file);

    // Close the file
    fclose(file);

    return 0;
}


int read_file(const char* filename, uint8_t images[][784], int num_images) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 8, SEEK_SET);
    uint32_t magic_number;
    fread(&magic_number, sizeof(uint32_t), 1, file);
    uint32_t total_images;
    fread(&total_images, sizeof(uint32_t), 1, file);

    printf("Magic Number: 0x%08X\n", magic_number);
    printf("Number of Images: %u\n", total_images);

    // Read the pixel data for the image
    for(int image_idx = 0; image_idx<num_images; image_idx++){
        fseek(file, 16 + (image_idx * 784), SEEK_SET);// Seek to the start of the specified image in the file
        fread(images[image_idx], sizeof(uint8_t), 784, file);
    }
    // Close the file
    fclose(file);
    return 0;
}

int main(){
    uint8_t images[600][784];
    if (read_file("train-images.idx3-ubyte", images, 600)!=0){
        return 1;
    }

    int k = 14;
    int M = 100;
    //int b = pow(2, k);

    bool ids[600][k];

    uint8_t query[784]; //we set query to be the first image from test set
    if (read_query("t10k-images.idx3-ubyte", query, 210)!=0){
        return 1;
    }

    for (int i = 0; i < 784; i++) {//print query image
        printf("%3u ", query[i]);
        if(i%28 == 0){
            printf("\n");
        }
    }
    printf("\n");


    bool qid[k];

    h_family hi(k, WINDOW, 784);
    for(int i=0; i<600; i++){
        hi.id(images[i], ids[i]);
    }
    /*for (int j=0; j<600; j++){
    for(int i=0; i<k; i++){
        cout<<ids[j][i]<<" ";
    }
    cout<<endl;
    }*/

    Hash hypercube(k, M);
    for(int i=0; i<600; i++){
        hypercube.insertItem(i, hypercube.hashFunction(ids[i]));
    }
    //hypercube.displayHash();

    hi.id(query, qid);
        for(int i=0; i<k; i++){
        cout<<qid[i]<<" ";
    }
    cout<<endl;
    int hash_index = hypercube.hashFunction(qid);
    hypercube.insertItem(601, hash_index);

    int options[M];

    int count = hypercube.getOptions(hash_index, options);
    int i = 1;
    while (count < M){//gather options until we have 10
        count += hypercube.getOptions(hash_index+i, &options[count]);
        count += hypercube.getOptions(hash_index-i, &options[count]);
        i++;
    }
    /*for (int i=0; i<count; i++){
        cout<<options[i]<<endl;
    }*/

    //FIND BEST 
    int best_index=-1;
    double best = numeric_limits<double>::max();

    for(int i=1; i<M; i++){
        if(dist(query, images[options[i]], 784)<best){
            best = (dist(query, images[options[i]], 784));
            best_index = options[i];
        }
    }

    //Print results
    if(best_index==-1){
        cout<<"Couldn't find NN"<<endl;
        return 1;
    }
    cout<<"Nearest neighbor is index "<<best_index<<" with dist="<<best<<endl;
    for (int i = 0; i < 784; i++) {
        printf("%3u ", images[best_index][i]);
        if(i%28 == 0){
            printf("\n");
        }
    }
    printf("\n");
}