/*
 * File: parallel_random_number_sorter.cpp
 * Author: Samuel Chamalé
 * Date: 8-29-2024
 *
 * Description:
 * This program demonstrates a parallel implementation of random number
 * generation, file I/O, and sorting using OpenMP. It performs the following tasks:
 * 1. Generates N random numbers in parallel and writes them to a CSV file.
 * 2. Reads the numbers from the file into memory.
 * 3. Sorts the numbers using a parallel implementation of QuickSort.
 * 4. Writes the sorted numbers to a new CSV file.
 *
 * The program showcases the use of OpenMP for parallelizing computationally
 * intensive tasks and measures the execution time for performance comparison
 * with the sequential version.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <chrono>

const char* INPUT_FILE = "random_numbers.csv";
const char* OUTPUT_FILE = "sorted_numbers.csv";

void generateRandomNumbers(int* numbers, int N, int maxValue) {
    #pragma omp parallel for
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

void parallelQuickSort(int* arr, int left, int right) {
    if (left < right) {
        int pivot = arr[right];
        int i = left - 1;

        for (int j = left; j < right; j++) {
            if (arr[j] <= pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[right]);

        int partition = i + 1;

        #pragma omp task
        parallelQuickSort(arr, left, partition - 1);

        #pragma omp task
        parallelQuickSort(arr, partition + 1, right);
    }
}

int main() {
    int N;
    std::cout << "Enter the number of random numbers to generate: ";
    std::cin >> N;

    srand(time(nullptr));  // Seed the random number generator

    // Allocate memory on the heap
    int* numbers = new int[N];

    auto start = std::chrono::high_resolution_clock::now();

    // Generate random numbers and write to file
    generateRandomNumbers(numbers, N, 1000);  // Generating numbers between 1 and 1000
    writeToFile(INPUT_FILE, numbers, N);

    // Read numbers from file
    readFromFile(INPUT_FILE, numbers, N);

    // Sort the numbers using parallel quicksort
    #pragma omp parallel
    {
        #pragma omp single
        parallelQuickSort(numbers, 0, N - 1);
    }

    // Write sorted numbers to output file
    writeToFile(OUTPUT_FILE, numbers, N);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Time taken: " << diff.count() << " seconds" << std::endl;

    // Clean up
    delete[] numbers;

    std::cout << "Random numbers have been generated, sorted, and written to files." << std::endl;
    std::cout << "Input file: " << INPUT_FILE << std::endl;
    std::cout << "Output file: " << OUTPUT_FILE << std::endl;

    return 0;
}
