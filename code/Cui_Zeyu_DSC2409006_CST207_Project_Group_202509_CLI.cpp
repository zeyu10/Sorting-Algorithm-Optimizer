/*
 * AI-Driven Sorting Algorithm Optimizer - Command Line Interface
 * g++ Cui_Zeyu_DSC2409006_CST207_Project_Group_202509_CLI.cpp -o SortingAlgorithmOptimizerCLI.exe -std=c++11
 * ./SortingAlgorithmOptimizerCLI
 */

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

using namespace std;

// ============= Data Structure Definitions =============

enum AlgoType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

struct DatasetFeatures {
    vector<int> data;
    int size;
    double sortedness;      // 0.0 (random) to 1.0 (sorted)
    int uniqueCount;        // Number of unique elements
    string type;            // Dataset type
    double reversedness;    // Degree of reverse order (0.0 ~ 1.0)
    double uniqueRatio;     // Ratio of unique elements (0.0 ~ 1.0)
    bool isLargeDataset;    // Large dataset indicator (>1000)
};

struct SortMetrics {
    long long comparisons = 0;      // Number of comparisons
    double executionTimeMs = 0.0;   // Execution time in milliseconds
    string algoName;
};

// ============= Sorting Algorithm Implementations =============

class SortingEngine {
public:
    // Bubble Sort Implementation
    static void bubbleSort(vector<int>& arr, long long& comparisons) {
        int n = arr.size();
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                comparisons++;
                if (arr[j] > arr[j + 1]) {
                    swap(arr[j], arr[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) break;  // Early termination if already sorted
        }
    }

    // Insertion Sort Implementation
    static void insertionSort(vector<int>& arr, long long& comparisons) {
        int n = arr.size();
        for (int i = 1; i < n; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= 0) {
                comparisons++;
                if (arr[j] <= key) break;
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    // Merge function for Merge Sort
    static void merge(vector<int>& arr, int l, int m, int r, long long& comparisons) {
        int n1 = m - l + 1;
        int n2 = r - m;
        vector<int> left(n1), right(n2);
        
        for (int i = 0; i < n1; i++) left[i] = arr[l + i];
        for (int j = 0; j < n2; j++) right[j] = arr[m + 1 + j];
        
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            comparisons++;
            if (left[i] <= right[j]) {
                arr[k++] = left[i++];
            } else {
                arr[k++] = right[j++];
            }
        }
        while (i < n1) arr[k++] = left[i++];
        while (j < n2) arr[k++] = right[j++];
    }

    // Merge Sort Implementation
    static void mergeSort(vector<int>& arr, int l, int r, long long& comparisons) {
        if (l >= r) return;
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m, comparisons);
        mergeSort(arr, m + 1, r, comparisons);
        merge(arr, l, m, r, comparisons);
    }

    // Partition function for Quick Sort
    static int partition(vector<int>& arr, int low, int high, long long& comparisons) {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            comparisons++;
            if (arr[j] < pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    // Quick Sort Implementation
    static void quickSort(vector<int>& arr, int low, int high, long long& comparisons) {
        if (low < high) {
            int pi = partition(arr, low, high, comparisons);
            quickSort(arr, low, pi - 1, comparisons);
            quickSort(arr, pi + 1, high, comparisons);
        }
    }

    // ============= Dataset Generation Functions =============
    
    // Generate random dataset
    static vector<int> generateRandomDataset(int size) {
        vector<int> arr(size);
        srand(time(nullptr));
        for (int i = 0; i < size; i++) {
            arr[i] = 1 + rand() % (size * 10);
        }
        return arr;
    }

    // Generate nearly sorted dataset
    static vector<int> generateNearlySorted(int size) {
        vector<int> arr(size);
        for (int i = 0; i < size; i++) arr[i] = i + 1;
        
        // Disorder about 10% of the elements
        int swaps = size / 10;
        srand(time(nullptr));
        for (int i = 0; i < swaps; i++) {
            int idx1 = rand() % size;
            int idx2 = rand() % size;
            swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }

    // Generate reversed dataset
    static vector<int> generateReversed(int size) {
        vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = size - i;
        }
        return arr;
    }

    // Generate dataset with few unique values
    static vector<int> generateFewUnique(int size, int uniqueCount) {
        vector<int> uniqueValues;
        srand(time(nullptr));
        for (int i = 0; i < uniqueCount; i++) {
            uniqueValues.push_back(rand() % 100 + 1);
        }
        
        vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = uniqueValues[rand() % uniqueCount];
        }
        return arr;
    }

    // ============= AI Analysis Module =============
    
    // Analyze dataset characteristics
    static DatasetFeatures analyzeDataset(const vector<int>& data) {
        DatasetFeatures features;
        features.size = data.size();
        features.data = data;
        features.isLargeDataset = (features.size > 1000);
        
        if (features.size <= 1) {
            features.sortedness = 1.0;
            features.reversedness = 0.0;
            features.uniqueCount = features.size;
            features.uniqueRatio = 1.0;
            features.type = "Single Element";
            return features;
        }
        
        // Calculate sortedness and reversedness
        long long ascendingPairs = 0, descendingPairs = 0;
        for (int i = 0; i < features.size - 1; i++) {
            if (data[i] <= data[i+1]) ascendingPairs++;
            if (data[i] >= data[i+1]) descendingPairs++;
        }
        
        features.sortedness = (double)ascendingPairs / (features.size - 1);
        features.reversedness = (double)descendingPairs / (features.size - 1);
        
        // Calculate uniqueness (sample first 1000 elements for performance)
        int sampleSize = min(1000, features.size);
        unordered_set<int> uniqueElements;
        for (int i = 0; i < sampleSize; i++) {
            uniqueElements.insert(data[i]);
        }
        
        features.uniqueCount = uniqueElements.size();
        features.uniqueRatio = (double)uniqueElements.size() / sampleSize;
        
        // Classify dataset type
        if (features.sortedness >= 0.90) features.type = "Nearly Sorted";
        else if (features.reversedness >= 0.90) features.type = "Reversed";
        else if (features.uniqueRatio < 0.40) features.type = "Few Unique";
        else if (features.isLargeDataset) features.type = "Large Random";
        else features.type = "Random";
        
        return features;
    }

    // Predict best sorting algorithm based on dataset features
    static AlgoType predictBestAlgorithm(const DatasetFeatures& features) {
        // AI Decision Tree based on algorithm complexity theory
        
        // Rule 1: Very small datasets (Size <= 50)
        // Insertion Sort has low constant factor, faster than Quick/Merge recursion overhead
        if (features.size <= 50) {
            return INSERTION_SORT;
        }
        
        // Rule 2: Large datasets (Size > 1000)
        if (features.isLargeDataset) {
            // Few unique values: Merge Sort is more stable than Quick Sort
            if (features.uniqueRatio < 0.40) {
                return MERGE_SORT;
            }
            return QUICK_SORT;
        }
        
        // Rule 3: Medium-sized datasets (50 < Size <= 1000)
        
        // Case A: Nearly sorted
        // Insertion Sort degrades to O(N) for nearly sorted data
        if (features.sortedness >= 0.90) {
            return INSERTION_SORT;
        }
        
        // Case B: Reversed
        // Quick Sort is better for reversed data
        if (features.reversedness >= 0.90) {
            return QUICK_SORT;
        }
        
        // Case C: Few unique values
        if (features.uniqueRatio < 0.40) {
            return MERGE_SORT;
        }
        
        // Case D: Random data
        return QUICK_SORT;
    }

    // Get algorithm name from type
    static string getAlgoName(AlgoType type) {
        switch (type) {
            case BUBBLE_SORT: return "Bubble Sort";
            case INSERTION_SORT: return "Insertion Sort";
            case MERGE_SORT: return "Merge Sort";
            case QUICK_SORT: return "Quick Sort";
            default: return "Unknown";
        }
    }

    // Run sorting algorithm and measure performance
    static SortMetrics runSort(AlgoType type, vector<int> data) {
        SortMetrics metrics;
        metrics.algoName = getAlgoName(type);
        metrics.comparisons = 0;
        
        auto start = chrono::high_resolution_clock::now();
        
        switch (type) {
            case BUBBLE_SORT: bubbleSort(data, metrics.comparisons); break;
            case INSERTION_SORT: insertionSort(data, metrics.comparisons); break;
            case MERGE_SORT: mergeSort(data, 0, data.size() - 1, metrics.comparisons); break;
            case QUICK_SORT: quickSort(data, 0, data.size() - 1, metrics.comparisons); break;
        }
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        metrics.executionTimeMs = duration.count();
        
        return metrics;
    }
};

// ============= Main Program =============

void printSeparator(char c = '=', int length = 70) {
    cout << string(length, c) << endl;
}

void displayMenu() {
    printSeparator();
    cout << "    AI-Driven Sorting Algorithm Optimizer" << endl;
    printSeparator();
    cout << "\nSelect Dataset Type:" << endl;
    cout << "  1. Random Dataset" << endl;
    cout << "  2. Nearly Sorted Dataset" << endl;
    cout << "  3. Reversed Dataset" << endl;
    cout << "  4. Few Unique Values Dataset" << endl;
    cout << "  0. Exit" << endl;
    printSeparator('-', 70);
}

void displayDataPreview(const vector<int>& data, int maxShow = 40) {
    cout << "\n[Data Preview]" << endl;
    cout << "Size: " << data.size() << " elements" << endl;
    cout << "First " << min(maxShow, (int)data.size()) << " elements: [";
    for (int i = 0; i < min(maxShow, (int)data.size()); i++) {
        cout << data[i];
        if (i < min(maxShow, (int)data.size()) - 1) cout << ", ";
    }
    if ((int)data.size() > maxShow) cout << ", ...";
    cout << "]" << endl;
}

void displayAnalysis(const DatasetFeatures& features, AlgoType predicted) {
    cout << "\n[AI Analysis Report]" << endl;
    printSeparator('-', 70);
    cout << "Dataset Characteristics:" << endl;
    cout << "  Type:         " << features.type << endl;
    cout << "  Size:         " << features.size 
         << (features.isLargeDataset ? " (Large Dataset)" : " (Small/Medium Dataset)") << endl;
    cout << "  Sortedness:   " << fixed << setprecision(1) 
         << (features.sortedness * 100.0) << "%" << endl;
    cout << "  Reversedness: " << (features.reversedness * 100.0) << "%" << endl;
    cout << "  Uniqueness:   " << (features.uniqueRatio * 100.0) 
         << "% (from sample)" << endl;
    cout << "  Unique Count: " << features.uniqueCount << endl;
    printSeparator('-', 70);
    cout << ">>> AI Predicted Best Algorithm: " 
         << SortingEngine::getAlgoName(predicted) << " <<<" << endl;
    printSeparator('-', 70);
}

void displayResults(const vector<SortMetrics>& results, const string& actualBest, const string& predicted) {
    cout << "\n[Sorting Performance Comparison]" << endl;
    printSeparator('-', 70);
    cout << left << setw(20) << "Algorithm"
         << setw(20) << "Comparisons"
         << setw(20) << "Time (ms)" << endl;
    printSeparator('-', 70);
    
    for (const auto& res : results) {
        cout << left << setw(20) << res.algoName;
        cout << setw(20) << res.comparisons;
        cout << setw(20) << fixed << setprecision(4) << res.executionTimeMs;
        
        if (res.algoName == actualBest) {
            cout << " <- FASTEST";
        }
        if (res.algoName == predicted) {
            cout << " [AI Predicted]";
        }
        cout << endl;
    }
    
    printSeparator('-', 70);
    cout << "Actual Best Algorithm: " << actualBest << endl;
    
    if (predicted == actualBest) {
        cout << "Result: AI Prediction was CORRECT!" << endl;
    } else {
        cout << "Result: AI Prediction was INCORRECT." << endl;
        cout << "  Predicted: " << predicted << endl;
        cout << "  Actual:    " << actualBest << endl;
    }
    printSeparator();
}

int main() {
    int choice, size, uniqueCount;
    vector<int> dataset;
    
    while (true) {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        
        if (choice == 0) {
            cout << "\nThank you for using the Sorting Algorithm Optimizer." << endl;
            break;
        }
        
        if (choice < 1 || choice > 4) {
            cout << "\nInvalid choice! Please select 1-4 or 0 to exit." << endl;
            continue;
        }
        
        cout << "Enter dataset size (10-100000): ";
        cin >> size;
        
        if (size < 10 || size > 100000) {
            cout << "\nInvalid size! Please enter a value between 10 and 100000." << endl;
            continue;
        }
        
        // Generate dataset based on user selection
        cout << "\nGenerating dataset..." << endl;
        try {
            switch(choice) {
                case 1:
                    dataset = SortingEngine::generateRandomDataset(size);
                    break;
                case 2:
                    dataset = SortingEngine::generateNearlySorted(size);
                    break;
                case 3:
                    dataset = SortingEngine::generateReversed(size);
                    break;
                case 4:
                    cout << "Enter number of unique values (2-50): ";
                    cin >> uniqueCount;
                    if (uniqueCount < 2) uniqueCount = 2;
                    if (uniqueCount > 50) uniqueCount = 50;
                    dataset = SortingEngine::generateFewUnique(size, uniqueCount);
                    break;
            }
            
            // Display preview
            displayDataPreview(dataset);
            
            // AI Analysis
            cout << "\nPerforming AI analysis..." << endl;
            DatasetFeatures features = SortingEngine::analyzeDataset(dataset);
            AlgoType predicted = SortingEngine::predictBestAlgorithm(features);
            displayAnalysis(features, predicted);
            
            // Run sorting algorithms
            cout << "\nRunning sorting algorithms..." << endl;
            vector<SortMetrics> results;
            
            // Skip O(n^2) algorithms for large datasets to save time
            if (size <= 1000) {
                cout << "  Running Bubble Sort..." << endl;
                results.push_back(SortingEngine::runSort(BUBBLE_SORT, dataset));
                cout << "  Running Insertion Sort..." << endl;
                results.push_back(SortingEngine::runSort(INSERTION_SORT, dataset));
            } else {
                cout << "  (Skipping O(n²) algorithms for large dataset)" << endl;
            }
            
            cout << "  Running Merge Sort..." << endl;
            results.push_back(SortingEngine::runSort(MERGE_SORT, dataset));
            cout << "  Running Quick Sort..." << endl;
            results.push_back(SortingEngine::runSort(QUICK_SORT, dataset));
            
            // Find the fastest algorithm
            string actualBest;
            double minTime = 1e9;
            for (const auto& r : results) {
                if (r.executionTimeMs < minTime) {
                    minTime = r.executionTimeMs;
                    actualBest = r.algoName;
                }
            }
            
            // Display results
            displayResults(results, actualBest, SortingEngine::getAlgoName(predicted));
            
            // Ask if user wants to continue
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            
        } catch (const exception& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }
    
    return 0;
}
