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
        // Τώρα εκτυπώστε τα pixel της εικόνας
        std::cout << "Εικόνα #" << i + 1 << ":" << std::endl;
        for (int row = 0; row < num_rows; row++) {
            for (int col = 0; col < num_cols; col++) {
                std::cout << static_cast<int>(image_data[row * num_cols + col]) << " ";
            }
            std::cout << std::endl;
        }
    }

    file.close();

    return 0;
}
