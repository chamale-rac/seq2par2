/*
 * File: optimized_parallel_random_number_sorter.cpp
 * Compile: g++ -fopenmp optimized_parallel_random_number_sorter.cpp -o optimized_parallel_sorter
 * Author: Samuel Chamalé
 * Date: 8-29-2024
 *
 * Description:
 * This program presents an optimized parallel implementation of random number
 * generation, file I/O, and sorting using OpenMP. It includes the following
 * enhancements over the basic parallel version:
 * 1. Dynamic adjustment of OpenMP thread count based on input size.
 * 2. Hybrid sorting algorithm: parallel QuickSort for large subarrays,
 *    sequential sorting for small subarrays.
 * 3. Optimized file I/O operations for large datasets.
 * 4. Fine-tuned OpenMP scheduling for random number generation.
 *
 * The program aims to achieve maximum performance through careful optimization
 * and parallelization strategies. It includes comprehensive timing measurements
 * for detailed performance analysis and comparison.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <vector>

const char* INPUT_FILE = "random_numbers.csv";
const char* OUTPUT_FILE = "sorted_numbers.csv";
const int SMALL_ARRAY_THRESHOLD = 1000;

void generateRandomNumbers(std::vector<int>& numbers, int maxValue) {
    #pragma omp parallel for schedule(guided)
    for (size_t i = 0; i < numbers.size(); i++) {
        numbers[i] = rand() % maxValue + 1;
    }
}

void writeToFile(const char* filename, const std::vector<int>& numbers) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < numbers.size() - 1; i++) {
        file << numbers[i] << ",";
    }
    file << numbers.back();

    file.close();
}

void readFromFile(const char* filename, std::vector<int>& numbers) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    int number;
    char comma;
    while (file >> number) {
        numbers.push_back(number);
        file >> comma;
    }

    file.close();
}

void sequentialQuickSort(std::vector<int>& arr, int left, int right) {
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
        sequentialQuickSort(arr, left, partition - 1);
        sequentialQuickSort(arr, partition + 1, right);
    }
}

void parallelQuickSort(std::vector<int>& arr, int left, int right) {
    if (right - left <= SMALL_ARRAY_THRESHOLD) {
        sequentialQuickSort(arr, left, right);
    } else if (left < right) {
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

    // Dynamic thread adjustment
    int num_threads = std::min(omp_get_max_threads(), 1 + N / 1000000);
    omp_set_num_threads(num_threads);

    std::vector<int> numbers(N);

    auto total_start = std::chrono::high_resolution_clock::now();

    // Generate random numbers
    auto gen_start = std::chrono::high_resolution_clock::now();
    generateRandomNumbers(numbers, 1000);  // Generating numbers between 1 and 1000
    auto gen_end = std::chrono::high_resolution_clock::now();

    // Write to file
    auto write_start = std::chrono::high_resolution_clock::now();
    writeToFile(INPUT_FILE, numbers);
    auto write_end = std::chrono::high_resolution_clock::now();

    // Read from file
    auto read_start = std::chrono::high_resolution_clock::now();
    readFromFile(INPUT_FILE, numbers);
    auto read_end = std::chrono::high_resolution_clock::now();

    // Sort the numbers using parallel quicksort
    auto sort_start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel
    {
        #pragma omp single
        parallelQuickSort(numbers, 0, N - 1);
    }
    auto sort_end = std::chrono::high_resolution_clock::now();

    // Write sorted numbers to output file
    auto write_sorted_start = std::chrono::high_resolution_clock::now();
    writeToFile(OUTPUT_FILE, numbers);
    auto write_sorted_end = std::chrono::high_resolution_clock::now();

    auto total_end = std::chrono::high_resolution_clock::now();

    // Calculate and print timings
    std::cout << "Time taken for generation: "
              << std::chrono::duration<double>(gen_end - gen_start).count() << " seconds" << std::endl;
    std::cout << "Time taken for writing to input file: "
              << std::chrono::duration<double>(write_end - write_start).count() << " seconds" << std::endl;
    std::cout << "Time taken for reading from input file: "
              << std::chrono::duration<double>(read_end - read_start).count() << " seconds" << std::endl;
    std::cout << "Time taken for sorting: "
              << std::chrono::duration<double>(sort_end - sort_start).count() << " seconds" << std::endl;
    std::cout << "Time taken for writing to output file: "
              << std::chrono::duration<double>(write_sorted_end - write_sorted_start).count() << " seconds" << std::endl;
    std::cout << "Total time taken: "
              << std::chrono::duration<double>(total_end - total_start).count() << " seconds" << std::endl;

    std::cout << "Random numbers have been generated, sorted, and written to files." << std::endl;
    std::cout << "Input file: " << INPUT_FILE << std::endl;
    std::cout << "Output file: " << OUTPUT_FILE << std::endl;

    return 0;
}
