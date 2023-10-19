#include <stdio.h>
#include <stdlib.h>

// Define the dimensions of the images
#define NUM_IMAGES 20
#define IMAGE_ROWS 28
#define IMAGE_COLS 28

// Function to create the .dat file
void createDatFile(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Header information
    int magic_number = 0x00000803;
    int num_images = NUM_IMAGES;
    int num_rows = IMAGE_ROWS;
    int num_cols = IMAGE_COLS;

    // Write the magic number, number of images, rows, and columns to the file
    fwrite(&magic_number, sizeof(int), 1, file);
    fwrite(&num_images, sizeof(int), 1, file);
    fwrite(&num_rows, sizeof(int), 1, file);
    fwrite(&num_cols, sizeof(int), 1, file);

    // Generate 20 images (adjust as needed)
    for (int i = 0; i < NUM_IMAGES; i++) {
        // Generate pixel values for the image (adjust as needed)
        unsigned char pixels[IMAGE_ROWS * IMAGE_COLS];
        for (int j = 0; j < IMAGE_ROWS * IMAGE_COLS; j++) {
            pixels[j] = i % 256; // 2 identical images
        }

        // Write the pixel values to the file
        fwrite(pixels, sizeof(unsigned char), IMAGE_ROWS * IMAGE_COLS, file);
    }

    fclose(file);
}

int main() {
    createDatFile("query.dat");
    printf("Created query.dat with 20 images.\n");

    // return the vector of image 0
    FILE *file = fopen("query.dat", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Skip the header and go to the start of the pixel data
    fseek(file, 16, SEEK_SET);

    // Read the pixel data for the image
    unsigned char image[IMAGE_ROWS * IMAGE_COLS];
    fread(image, sizeof(unsigned char), IMAGE_ROWS * IMAGE_COLS, file);

    // Close the file

    // Display the pixel values of the image as a vector
    printf("Pixel values of Image 0 as a vector:\n");
    for (int i = 0; i < IMAGE_ROWS * IMAGE_COLS; i++) {
        printf("%3u ", image[i]);
    }
    printf("\n");

    // display the vector of image 1
    fseek(file, 16 + (1 * IMAGE_ROWS * IMAGE_COLS), SEEK_SET);
    fread(image, sizeof(unsigned char), IMAGE_ROWS * IMAGE_COLS, file);
    printf("Pixel values of Image 3 as a vector:\n");
    for (int i = 0; i < IMAGE_ROWS * IMAGE_COLS; i++) {
        printf("%3u ", image[i]);
    }
    printf("\n");
    
    return 0;
}
