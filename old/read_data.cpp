#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::ifstream file("mnist_dataset.dat", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Αδυναμία ανοίγματος αρχείου." << std::endl;
        return 1;
    }

    int magic_number, num_images, num_rows, num_cols;

    // Διαβάστε τον μαγικό αριθμό (magic number)
    file.read(reinterpret_cast<char*>(&magic_number), sizeof(int));

    // Διαβάστε τον αριθμό των εικόνων
    file.read(reinterpret_cast<char*>(&num_images), sizeof(int));

    // Διαβάστε τον αριθμό των γραμμών και στηλών
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(int));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(int));

    std::vector<unsigned char> image_data(num_rows * num_cols);
    for (int i = 0; i < num_images; i++) {
        file.read(reinterpret_cast<char*>(image_data.data()), num_rows * num_cols);
        // Τώρα έχετε τα δεδομένα της εικόνας στο vector image_data
    }
    printf("image_data: %d\n", image_data[0]);
    printf( "magic number: %d\n", magic_number );
    printf( "num_images: %d\n", num_images );
    printf( "num_rows: %d\n", num_rows );
    printf( "num_cols: %d\n", num_cols );
    

    file.close();

    return 0;
}
