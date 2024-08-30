/*
 * File: sequential_random_number_sorter.cpp
 * Compile: g++ sequential_random_number_sorter.cpp -o sequential_random_number_sorter
 * Author: Samuel Chamalé
 * Date: 8-29-2024
 *
 * Description:
 * This program demonstrates a sequential implementation of random number
 * generation, file I/O, and sorting. It performs the following tasks:
 * 1. Generates N random numbers and writes them to a CSV file.
 * 2. Reads the numbers from the file into memory.
 * 3. Sorts the numbers using the C++ standard library's sort function.
 * 4. Writes the sorted numbers to a new CSV file.
 *
 * The program uses dynamic memory allocation to handle variable-sized inputs
 * and demonstrates basic file handling in C++.
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const char* INPUT_FILE = "random_numbers.csv";
const char* OUTPUT_FILE = "sorted_numbers.csv";

void generateRandomNumbers(int* numbers, int N, int maxValue) {
    for (int i = 0; i < N; i++) {
        numbers[i] = rand() % maxValue + 1;
    }
}

void writeToFile(const char* filename, int* numbers, int N) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    for (int i = 0; i < N - 1; i++) {
        file << numbers[i] << ",";
    }
    file << numbers[N - 1];

    file.close();
}

void readFromFile(const char* filename, int* numbers, int N) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (file.peek() == EOF) break;
        file >> numbers[i];
        if (file.peek() == ',') file.ignore();
    }

    file.close();
}

int main() {
    int N;
    std::cout << "Enter the number of random numbers to generate: ";
    std::cin >> N;

    srand(time(nullptr));  // Seed the random number generator

    // Allocate memory on the heap
    int* numbers = new int[N];

    // Generate random numbers and write to file
    generateRandomNumbers(numbers, N, 1000);  // Generating numbers between 1 and 1000
    writeToFile(INPUT_FILE, numbers, N);

    // Read numbers from file
    readFromFile(INPUT_FILE, numbers, N);

    // Sort the numbers
    std::sort(numbers, numbers + N);

    // Write sorted numbers to output file
    writeToFile(OUTPUT_FILE, numbers, N);

    // Clean up
    delete[] numbers;

    std::cout << "Random numbers have been generated, sorted, and written to files." << std::endl;
    std::cout << "Input file: " << INPUT_FILE << std::endl;
    std::cout << "Output file: " << OUTPUT_FILE << std::endl;

    return 0;
}
