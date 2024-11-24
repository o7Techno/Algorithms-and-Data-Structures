#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>

class MergeSorting {
  public:
    static void Merge(std::vector<int>& values, std::vector<int>& left, std::vector<int>& right) {
        size_t n1 = left.size();
        size_t n2 = right.size();

        size_t i = 0;
        size_t j = 0;
        size_t k = 0;

        while (i < n1 && j < n2) {
            if (left[i] <= right[j]) {
                values[k] = left[i];
                i++;
            } else {
                values[k] = right[j];
                j++;
            }
            k++;
        }

        for (; i < n1; ++i, ++k) {
            values[k] = left[i];
        }
        for (; j < n2; ++j, ++k) {
            values[k] = right[j];
        }
    }


    static void MergeSort(std::vector<int>& values) {
        if (values.size() <= 1) {
            return;
        }
        std::vector<int> left(values.begin(), values.begin() + values.size() / 2);
        MergeSort(left);
        std::vector<int> right(values.begin() + values.size() / 2, values.end());
        MergeSort(right);
        Merge(values, left, right);
    }
};

class MergeInsertSort {
  public:
    static void MergeInsert(std::vector<int>& values, std::vector<int>& left, std::vector<int>& right) {
        size_t n1 = left.size();
        size_t n2 = right.size();

        size_t i = 0;
        size_t j = 0;
        size_t k = 0;

        while (i < n1 && j < n2) {
            if (left[i] <= right[j]) {
                values[k] = left[i];
                i++;
            } else {
                values[k] = right[j];
                j++;
            }
            k++;
        }

        for (; i < n1; ++i, ++k) {
            values[k] = left[i];
        }
        for (; j < n2; ++j, ++k) {
            values[k] = right[j];
        }
    }

    static void InsertionSort(std::vector<int>& values) {
        for (size_t i = 1; i < values.size(); ++i) {
            int key = values[i];
            int j = i - 1;

            while (j >= 0 && values[j] > key) {
                values[j + 1]  = values[j];
                j = j - 1;
            }
            values[j + 1] = key;
        }
    }


    static void MergeInsertionSort(std::vector<int>& values) {
        if (values.size() <= 50) {
            return InsertionSort(values);
        }
        std::vector<int> left(values.begin(), values.begin() + values.size() / 2);
        MergeInsertionSort(left);
        std::vector<int> right(values.begin() + values.size() / 2, values.end());
        MergeInsertionSort(right);
        MergeInsert(values, left, right);
    }
};

class ArrayGenerator {
  public:
    ArrayGenerator() : random_array(10000) {
        GenerateArrays();
    }
    void GenerateArrays(){
        for (size_t i = 0; i < 10000; ++i) {
            random_array.push_back(std::rand() % 6001);
        }
        reversed_sorted_array = random_array;
        std::sort(reversed_sorted_array.begin(), reversed_sorted_array.end(), std::greater());
        almost_sorted_array = random_array;
        std::sort(almost_sorted_array.begin(), almost_sorted_array.end());
    }

    std::vector<int> GetRandomArray(size_t length) {
        return std::vector<int> (random_array.begin(), random_array.begin() + length);
    }

    std::vector<int> GetReversedSortedArray(size_t length) {
        return std::vector<int> (reversed_sorted_array.begin(), reversed_sorted_array.begin() + length);
    }

    std::vector<int> GetAlmostSortedArray(size_t length) {
        std::vector<int> result(almost_sorted_array.begin(), almost_sorted_array.begin() + length);
        for (size_t i = 0; i < length / 10; ++i) {
            size_t idx1 = rand() % length;
            size_t idx2 = rand() % length;
            std::swap(result[idx1], result[idx2]);
        }
        return result;
    }
  private:
    std::vector<int> random_array;
    std::vector<int> reversed_sorted_array;
    std::vector<int> almost_sorted_array;
};


class SortTester {
  public:
    SortTester() : generator(){}
    std::vector<double> MergeSortTestRandomArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetRandomArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeSorting::MergeSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);
        }
        return results;
    }

    std::vector<double> MergeSortTestReversedSortedArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetReversedSortedArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeSorting::MergeSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);

        }
        return results;
    }

    std::vector<double> MergeSortTestAlmostSortedArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetAlmostSortedArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeSorting::MergeSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);

        }
        return results;
    }

        std::vector<double> MergeInsertSortTestRandomArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetRandomArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeInsertSort::MergeInsertionSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);
        }
        return results;
    }

    std::vector<double> MergeInsertSortTestReversedSortedArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetReversedSortedArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeInsertSort::MergeInsertionSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);

        }
        return results;
    }

    std::vector<double> MergeInsertSortTestAlmostSortedArrays() {
        std::vector<double> results;
        for (size_t length = 500; length <= 10000; length += 100) {
            std::vector<int> array = generator.GetAlmostSortedArray(length);
            double total_time = 0;
            for (size_t time = 0; time < 10; ++time) {
                auto start = std::chrono::high_resolution_clock::now();
                MergeInsertSort::MergeInsertionSort(array);
                auto elapsed = std::chrono::high_resolution_clock::now()- start;
                total_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            }
            results.push_back(total_time / 10);

        }
        return results;
    }
  private:
    ArrayGenerator generator;
};



int main () {
    SortTester tester{};
    std::vector<double> random_merge_test_results = tester.MergeSortTestRandomArrays();
    std::vector<double> reverse_merge_test_results = tester.MergeSortTestReversedSortedArrays();
    std::vector<double> almost_merge_test_results = tester.MergeSortTestAlmostSortedArrays();

    std::vector<double> random_merge_insert_test_results = tester.MergeInsertSortTestRandomArrays();
    std::vector<double> reverse_merge_insert_test_results = tester.MergeInsertSortTestReversedSortedArrays();
    std::vector<double> almost_merge_insert_test_results = tester.MergeInsertSortTestAlmostSortedArrays();

    std::ofstream res("sorting_res_threshold50.csv");

    res << "Array length;Merge Sort Random;Merge Sort Reverse Sorted;Merge Sort Almost Sorted;Merge+Insertion Sort Random;Merge+Insertion Sort Reverse Sorted;Merge+Insertion Almost Sorted\n";

    for (size_t i = 0; i < random_merge_insert_test_results.size(); ++i) {
        res << 500 + i * 100 << ";" << random_merge_test_results[i] << ";" << reverse_merge_test_results[i] << ";" << almost_merge_test_results[i] 
        << ";" << random_merge_insert_test_results[i] << ";" << reverse_merge_insert_test_results[i] << ";" << almost_merge_insert_test_results[i] << "\n";
    }
    return 0;
}