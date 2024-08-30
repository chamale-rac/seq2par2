/*
 * File: performance_analysis.cpp
 * Compile: g++ -fopenmp performance_analysis.cpp -o performance_analysis
 * Author: Samuel Chamalé
 * Date: 8-29-2024
 *
 * Description:
 * This program is designed to analyze and compare the performance of the
 * sequential, parallel, and optimized parallel implementations of the random
 * number sorter. It performs the following tasks:
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
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <iomanip>

// Function prototypes for the different implementations
void sequentialSort(std::vector<int>& numbers);
void parallelSort(std::vector<int>& numbers);
void optimizedParallelSort(std::vector<int>& numbers);

// Helper function to generate random numbers
void generateRandomNumbers(std::vector<int>& numbers, int maxValue) {
    for (size_t i = 0; i < numbers.size(); i++) {
        numbers[i] = rand() % maxValue + 1;
    }
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

    std::ofstream reportFile("performance_report.csv");
    reportFile << "Input Size,Sequential Time,Parallel Time,Optimized Parallel Time,Parallel Speedup,Optimized Speedup" << std::endl;

    for (int size : inputSizes) {
        double seqTotalTime = 0, parTotalTime = 0, optParTotalTime = 0;

        for (int run = 0; run < numRuns; run++) {
            std::vector<int> numbers(size);
            generateRandomNumbers(numbers, 1000000);

            std::vector<int> seqNumbers = numbers;
            std::vector<int> parNumbers = numbers;
            std::vector<int> optParNumbers = numbers;

            double seqTime = measureExecutionTime(sequentialSort, seqNumbers);
            double parTime = measureExecutionTime(parallelSort, parNumbers);
            double optParTime = measureExecutionTime(optimizedParallelSort, optParNumbers);

            seqTotalTime += seqTime;
            parTotalTime += parTime;
            optParTotalTime += optParTime;
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
    std::cout << "Performance report has been written to performance_report.csv" << std::endl;

    return 0;
}

// Placeholder implementations of sorting functions
void sequentialSort(std::vector<int>& numbers) {
    std::sort(numbers.begin(), numbers.end());
}

void parallelSort(std::vector<int>& numbers) {
    #pragma omp parallel
    {
        #pragma omp single
        std::sort(numbers.begin(), numbers.end());
    }
}

void optimizedParallelSort(std::vector<int>& numbers) {
    int num_threads = std::min(omp_get_max_threads(), 1 + (int)numbers.size() / 1000000);
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        #pragma omp single
        std::sort(numbers.begin(), numbers.end());
    }
}
