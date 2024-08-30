/*
 * File: complete_performance_analysis.cpp
 * Compile: g++ -fopenmp complete_performance_analysis.cpp -o complete_performance_analysis
 * Author: Samuel Chamalé
 * Date: 8-29-2024
 *
 * Description:
 * This program is designed to analyze and compare the performance of the
 * sequential, parallel, and improved parallel implementations of the quicksort algorithm.
 * It performs the following tasks:
 * 1. Runs each implementation multiple times with varying input sizes.
 * 2. Measures and records execution times for each run.
 * 3. Calculates speedup and efficiency metrics.
 * 4. Generates performance reports and graphs.
 *
 * The program helps in understanding the scalability and efficiency of the
 * parallel implementations compared to the sequential version across different
 * problem sizes and system configurations.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <random>

const int SMALL_ARRAY_THRESHOLD = 1000;
const int SEQUENTIAL_THRESHOLD = 1000;

// Sequential QuickSort implementation
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

// Optimized Parallel QuickSort implementation
void optimizedParallelQuickSort(std::vector<int>& arr, int left, int right) {
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

        #pragma omp task shared(arr)
        optimizedParallelQuickSort(arr, left, partition - 1);

        #pragma omp task shared(arr)
        optimizedParallelQuickSort(arr, partition + 1, right);

        #pragma omp taskwait  // Ensure that tasks complete before proceeding
    }
}

// Parallel QuickSort implementation with improved synchronization
void parallelQuickSort(std::vector<int>& arr, int low, int high, int depth) {
    if (high - low < SEQUENTIAL_THRESHOLD || depth > 3) {
        sequentialQuickSort(arr, low, high);
        return;
    }

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);

    int pi = i + 1;

    #pragma omp task shared(arr) if(depth <= 3)
    parallelQuickSort(arr, low, pi - 1, depth + 1);

    #pragma omp task shared(arr) if(depth <= 3)
    parallelQuickSort(arr, pi + 1, high, depth + 1);

    #pragma omp taskwait  // Ensure that tasks complete before proceeding
}

void parallelSort(std::vector<int>& arr) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        parallelQuickSort(arr, 0, arr.size() - 1, 0);
    }
}

// Wrapper functions
void sequentialSort(std::vector<int>& numbers) {
    std::sort(numbers.begin(), numbers.end());
}

void optimizedParallelSort(std::vector<int>& numbers) {
    #pragma omp parallel
    {
        #pragma omp single
        optimizedParallelQuickSort(numbers, 0, numbers.size() - 1);
    }
}

void parallelOptimizedSort(std::vector<int>& numbers) {
    parallelSort(numbers);
}

// Helper function to generate random numbers
std::vector<int> generateRandomVector(int size) {
    std::vector<int> vec(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);

    for (int& num : vec) {
        num = dis(gen);
    }
    return vec;
}

// Function to measure execution time of a sorting function
double measureExecutionTime(void (*sortFunction)(std::vector<int>&), std::vector<int>& numbers) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunction(numbers);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    srand(time(nullptr));  // Seed the random number generator

    std::vector<int> inputSizes = {10000, 100000, 1000000, 10000000};
    int numRuns = 5;

    std::ofstream reportFile("complete_performance_report.csv");
    reportFile << "Input Size,Sequential Time,Parallel Time,Optimized Parallel Time,Parallel Speedup,Optimized Speedup" << std::endl;

    for (int size : inputSizes) {
        double seqTotalTime = 0, parTotalTime = 0, optParTotalTime = 0;

        for (int run = 0; run < numRuns; run++) {
            std::vector<int> numbers = generateRandomVector(size);

            std::vector<int> seqNumbers = numbers;
            std::vector<int> parNumbers = numbers;
            std::vector<int> optParNumbers = numbers;

            seqTotalTime += measureExecutionTime(sequentialSort, seqNumbers);
            parTotalTime += measureExecutionTime(parallelOptimizedSort, parNumbers);
            optParTotalTime += measureExecutionTime(optimizedParallelSort, optParNumbers);

            if (!std::equal(seqNumbers.begin(), seqNumbers.end(), parNumbers.begin())) {
                std::cerr << "Error: Parallel sort produced incorrect results for size " << size << std::endl;
                return 1;
            }

            if (!std::equal(seqNumbers.begin(), seqNumbers.end(), optParNumbers.begin())) {
                std::cerr << "Error: Optimized parallel sort produced incorrect results for size " << size << std::endl;
                return 1;
            }
        }

        double seqAvgTime = seqTotalTime / numRuns;
        double parAvgTime = parTotalTime / numRuns;
        double optParAvgTime = optParTotalTime / numRuns;

        double parSpeedup = seqAvgTime / parAvgTime;
        double optParSpeedup = seqAvgTime / optParAvgTime;

        reportFile << size << ","
                   << seqAvgTime << ","
                   << parAvgTime << ","
                   << optParAvgTime << ","
                   << parSpeedup << ","
                   << optParSpeedup << std::endl;

        std::cout << "Input size: " << size << std::endl;
        std::cout << "Sequential avg time: " << seqAvgTime << " seconds" << std::endl;
        std::cout << "Parallel avg time: " << parAvgTime << " seconds" << std::endl;
        std::cout << "Optimized parallel avg time: " << optParAvgTime << " seconds" << std::endl;
        std::cout << "Parallel speedup: " << parSpeedup << std::endl;
        std::cout << "Optimized parallel speedup: " << optParSpeedup << std::endl;
        std::cout << std::endl;
    }

    reportFile.close();
    std::cout << "Performance report has been written to complete_performance_report.csv" << std::endl;

    return 0;
}
