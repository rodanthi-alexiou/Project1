#include <stdio.h>
#include <stdint.h>

int main() {
    FILE *file = fopen("train-images.idx3-ubyte", "rb");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Offset and type are 32-bit integers, but we don't need them here
    fseek(file, 8, SEEK_SET); // Skip offset and type

    // Read the value (magic number)
    uint32_t magic_number;
    fread(&magic_number, sizeof(uint32_t), 1, file);

    // Read the description
    uint32_t description;
    fread(&description, sizeof(uint32_t), 1, file);

    // Read the number of images, rows, and columns
    uint32_t num_images, num_rows, num_columns;
    fread(&num_images, sizeof(uint32_t), 1, file);
    fread(&num_rows, sizeof(uint32_t), 1, file);
    fread(&num_columns, sizeof(uint32_t), 1, file);

    printf("Magic Number: 0x%08X\n", magic_number);
    printf("Number of Images: %u\n", num_images);
    printf("Number of Rows: %u\n", num_rows);
    printf("Number of Columns: %u\n", num_columns);

    // Index of the image you want to retrieve (0-based)
    int image_index = 3;  // Change this index to get a different image

    // Seek to the start of the specified image in the file
    fseek(file, 16 + (image_index * 784), SEEK_SET);

    // Read the pixel data for the image
    uint8_t image[784]; // 28 x 28 = 784
    fread(image, sizeof(uint8_t), 784, file);

    // Close the file
    fclose(file);

    // Now, the 'image' array contains the pixel data for the specified image
    // You can process and display this data as needed

    // Display the pixel values of the image as a vector
    printf("Pixel values of Image %d as a vector:\n", image_index);
    for (int i = 0; i < 784; i++) {
        printf("%3u ", image[i]);
    }
    printf("\n");

    return 0;
}
