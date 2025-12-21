#include <bits/stdc++.h>
using namespace std;

enum AlgoType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

struct DatasetFeatures {
    vector<int> data;
    int size;
    double sortedness;    // 0.0 (random) to 1.0 (sorted)
    int uniqueCount;      // Number of unique elements
    string type;          // Dataset type
    double reversedness;  // Reverse degree (0.0 ~ 1.0)
    double uniqueRatio;   // Uniqueness ratio (0.0 ~ 1.0)
    bool isLargeDataset;  // Large dataset indicator (>1000)
};

struct SortMetrics {
    long long comparisons = 0;      // Number of comparisons
    double executionTimeMs = 0.0;   // Execution time in milliseconds
    string algoName;
};

// Task 1: Dataset Generation
vector<int> generateRandomDataset(int size,
                             int minVal = numeric_limits<int>::min(), 
                             int maxVal = numeric_limits<int>::max(),
                             bool printInfo = true,
                             int printLimit = 10) {
    
    // Strict parameter checking
    if (size <= 0) {
        throw invalid_argument("Array size must be positive. Got: " + to_string(size));
    }
    
    if (minVal > maxVal) {
        throw invalid_argument("Minimum value (" + to_string(minVal) + 
                              ") cannot be greater than maximum value (" + 
                              to_string(maxVal) + ")");
    }
    
    // Initialize random seed
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    // Generate array
    vector<int> arr;
    
    // For very large ranges, use better distribution
    long long range = static_cast<long long>(maxVal) - minVal + 1;
    
    if (range > RAND_MAX) {
        // Use multiple rand() calls for large ranges
        arr.reserve(size);
        for (int i = 0; i < size; i++) {
            // Calculate how many rand() calls we need
            int parts = 1;
            long long current_max = RAND_MAX;
            while (range > current_max) {
                parts++;
                current_max = current_max * (RAND_MAX + 1LL);
            }
            
            long long random_val = 0;
            for (int j = 0; j < parts; j++) {
                random_val = random_val * (RAND_MAX + 1LL) + rand();
            }
            
            arr.push_back(minVal + static_cast<int>(random_val % range));
        }
    } else {
        // Standard case for smaller ranges
        arr.reserve(size);
        int int_range = static_cast<int>(range);
        for (int i = 0; i < size; i++) {
            arr.push_back(minVal + rand() % int_range);
        }
    }
    
    if (!printInfo) {
        return arr;
    }
    
    DatasetFeatures features;
    features.data = arr;
    features.size = size;
    
    // Calculate sortedness
    if (arr.size() < 2) {
        features.sortedness = 0.5;
    } else {
        long long inversions = 0;
        int n = arr.size();
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (arr[i] > arr[j]) inversions++;
            }
        }
        
        long long maxInversions = static_cast<long long>(n) * (n - 1) / 2;
        
        if (maxInversions == 0) {
            features.sortedness = 0.5;
        } else {
            double inversionRatio = static_cast<double>(inversions) / maxInversions;
            features.sortedness = 1.0 - 2.0 * fabs(inversionRatio - 0.5);
            features.sortedness = max(0.0, min(1.0, features.sortedness));
        }
    }
    
    unordered_set<int> uniqueSet(arr.begin(), arr.end());
    features.uniqueCount = uniqueSet.size();
    
    // Set dataset type
    string valueRange = "[" + to_string(minVal) + ", " + to_string(maxVal) + "]";
    if (minVal == numeric_limits<int>::min() && maxVal == numeric_limits<int>::max()) {
        features.type = "Completely Random Array (full int range)";
    } else if (range <= 100) {
        features.type = "Random Array with Limited Range " + valueRange;
    } else {
        features.type = "Random Array with Range " + valueRange;
    }
    
    cout << "=== Generated Random Dataset ===" << endl;
    cout << "Type: " << features.type << endl;
    cout << "Size: " << features.size << " elements" << endl;
    cout << "Sortedness: " << fixed << setprecision(3) << features.sortedness 
         << " (0=random, 1=sorted)" << endl;
    cout << "Unique values: " << features.uniqueCount 
         << " (" << fixed << setprecision(1) 
         << (features.size > 0 ? (100.0 * features.uniqueCount / features.size) : 0.0)
         << "% of total)" << endl;
    
    cout << "Array preview [" << min(printLimit, size) << "/" << size << "]: [";
    
    int show = min(printLimit, size);
    for (int i = 0; i < show; i++) {
        cout << arr[i];
        if (i < show - 1) cout << ", ";
    }
    
    if (size > printLimit) {
        cout << ", ... (" << size - printLimit << " more)";
    }
    cout << "]" << endl;
    
    cout << "=================================" << endl;
    
    return arr;
}
vector<int> generateNearlySorted(int size, float disorderPercent = 0.1, bool printInfo = true) {
    if (size <= 0) {
        throw invalid_argument("Array size must be positive. Got: " + to_string(size));
    }
    
    // Limit disorder percentage between 0-100%
    if (disorderPercent < 0) disorderPercent = 0;
    if (disorderPercent > 1.0) disorderPercent = 1.0;
    
    // Initialize random seed
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    // Create sorted array
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;
    }
    
    // 2. Calculate number of elements to disorder
    int disorderCount = static_cast<int>(size * disorderPercent);
    
    if (disorderCount > 0) {
        vector<int> indices(size);
        for (int i = 0; i < size; i++) indices[i] = i;
        
        // Shuffle indices
        for (int i = size - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(indices[i], indices[j]);
        }
        
        // Modify first disorderCount elements
        for (int i = 0; i < disorderCount; i++) {
            int randomIndex = indices[i];
            int randomValue = 1 + rand() % (size * 2);
            arr[randomIndex] = randomValue;
        }
    }
    
    if (!printInfo) {
        return arr;
    }
    
    DatasetFeatures features;
    features.data = arr;
    features.size = size;
    
    if (arr.size() < 2) {
        features.sortedness = 1.0;
    } else {
        long long inversions = 0;
        int n = arr.size();
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (arr[i] > arr[j]) inversions++;
            }
        }
        
        long long maxInversions = static_cast<long long>(n) * (n - 1) / 2;
        
        if (maxInversions == 0) {
            features.sortedness = 1.0;
        } else {
            features.sortedness = 1.0 - (static_cast<double>(inversions) / maxInversions);
        }
    }
    
    unordered_set<int> uniqueSet(arr.begin(), arr.end());
    features.uniqueCount = uniqueSet.size();
    cout << "=== Generated Nearly Sorted Array ===" << endl;
    cout << "Type: " << features.type << endl;
    cout << "Size: " << features.size << " elements" << endl;
    cout << "Disorder level: " << static_cast<int>(disorderPercent * 100) << "%" << endl;
    cout << "Sortedness: " << features.sortedness << " (0=random, 1=sorted)" << endl;
    cout << "Unique values: " << features.uniqueCount << endl;
    
    // 打印数组内容
    int show = min(10, size);
    cout << "Data (first " << show << "): [";
    for (int i = 0; i < show; i++) {
        cout << arr[i];
        if (i < show - 1) cout << ", ";
    }
    if (size > show) {
        cout << ", ... " << size - show << " more";
    }
    cout << "]" << endl;
    cout << "===================================" << endl;
    
    return arr;
}

vector<int> generateReversed(int size, int printLimit = 10) {     
    if (size <= 0) {         
        throw invalid_argument("Array size must be positive. Got: " + to_string(size));
    }          
    
    vector<int> arr(size);
    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(1, size * 10);
    
    for (int i = 0; i < size; i++) {
        arr[i] = distribution(generator);
    }
    
    sort(arr.begin(), arr.end(), greater<int>());
    
    // Calculate features
    DatasetFeatures features;
    features.data = arr;
    features.size = size;
    features.type = "Random descending array";
    
    if (size < 2) {
        features.sortedness = 1.0;
    } else {
        int correct = 0;
        int totalPairs = size * (size - 1) / 2;
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                if (arr[i] >= arr[j]) correct++;
            }
        }
        features.sortedness = (double)correct / totalPairs;
    }
    
    unordered_set<int> uniqueSet(arr.begin(), arr.end());
    features.uniqueCount = uniqueSet.size();
    if (arr.empty()) {         
        cout << "[] (empty array)" << endl;     
    } else {         
        cout << "[";         
        int show = min(size, printLimit);                  
        
        for (int i = 0; i < show; i++) {             
            cout << arr[i];             
            if (i < show - 1) cout << ", ";         
        }                  
        
        if (size > printLimit) {             
            cout << ", ... (" << size - printLimit << " more)";         
        }                  
        
        cout << "]" << endl;     
    }          
    
    return arr;
}

vector<int> generateFewUniqueArray(int size, int uniqueCount = 5, 
                                  bool printArray = true, 
                                  int printLimit = 10) {
    if (size <= 0) {
        throw invalid_argument("Array size must be positive. Got: " + to_string(size));
    }
    
    if (uniqueCount <= 0) {
        throw invalid_argument("Unique count must be positive. Got: " + to_string(uniqueCount));
    }
    
    if (uniqueCount > size) {
        throw invalid_argument("Unique count (" + to_string(uniqueCount) + 
                              ") cannot exceed array size (" + to_string(size) + ")");
    }
    
    // Initialize random seed
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    // Generate unique values set
    unordered_set<int> uniqueSet;
    vector<int> uniqueValues;
    
    int maxRandomValue = 1000000;
    
    for (int i = 0; i < uniqueCount; i++) {
        int attempts = 0;
        const int MAX_ATTEMPTS = 1000;
        bool valueAdded = false;
        
        while (attempts < MAX_ATTEMPTS && !valueAdded) {
            int randomValue = rand() % maxRandomValue + 1;
            if (uniqueSet.find(randomValue) == uniqueSet.end()) {
                uniqueSet.insert(randomValue);
                uniqueValues.push_back(randomValue);
                valueAdded = true;
            }
            attempts++;
        }
        
        // Fallback strategy if random generation fails
        if (!valueAdded) {
            int baseValue = 10000 * (i + 1);
            int offset = rand() % 1000;
            int fallbackValue = baseValue + offset;
            
            while (uniqueSet.find(fallbackValue) != uniqueSet.end()) {
                fallbackValue++;
            }
            
            uniqueSet.insert(fallbackValue);
            uniqueValues.push_back(fallbackValue);
        }
    }
    
    // Fill array with these unique values
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        int randomIndex = rand() % uniqueValues.size();
        arr[i] = uniqueValues[randomIndex];
    }
    
    if (!printArray) {
        return arr;
    }
    
    DatasetFeatures features;
    features.data = arr;
    features.size = size;
    
    if (arr.size() < 2) {
        features.sortedness = 0.5;
    } else {
        long long inversions = 0;
        int n = arr.size();
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (arr[i] > arr[j]) inversions++;
            }
        }
        
        long long maxInversions = static_cast<long long>(n) * (n - 1) / 2;
        
        if (maxInversions == 0) {
            features.sortedness = 0.5;
        } else {
            double inversionRatio = static_cast<double>(inversions) / maxInversions;
            features.sortedness = 1.0 - 2.0 * fabs(inversionRatio - 0.5);
            features.sortedness = max(0.0, min(1.0, features.sortedness));
        }
    }
    
    unordered_set<int> actualUniqueSet(arr.begin(), arr.end());
    features.uniqueCount = actualUniqueSet.size();
    cout << "=== Generated Few Unique Array ===" << endl;
    cout << "Type: " << features.type << endl;
    cout << "Size: " << features.size << " elements" << endl;
    cout << "Requested unique values: " << uniqueCount << endl;
    cout << "Actual unique values: " << features.uniqueCount << endl;
    cout << "Sortedness: " << fixed << setprecision(3) << features.sortedness 
         << " (0=random, 1=sorted)" << endl;
    
    sort(uniqueValues.begin(), uniqueValues.end());
    cout << "Generated unique values (" << uniqueValues.size() << "): [";
    int showUnique = min(5, (int)uniqueValues.size());
    for (int i = 0; i < showUnique; i++) {
        cout << uniqueValues[i];
        if (i < showUnique - 1) cout << ", ";
    }
    if (uniqueValues.size() > 5) {
        cout << ", ... " << uniqueValues.size() - 5 << " more";
    }
    cout << "]" << endl;
    
    int show = min(printLimit, size);
    cout << "Array preview [" << show << "/" << size << "]: [";
    for (int i = 0; i < show; i++) {
        cout << arr[i];
        if (i < show - 1) cout << ", ";
    }
    
    if (size > printLimit) {
        cout << ", ... (" << size - printLimit << " more)";
    }
    cout << "]" << endl;
    
    if (show >= 5) {
        unordered_set<int> previewUniqueSet;
        for (int i = 0; i < show; i++) {
            previewUniqueSet.insert(arr[i]);
        }
        cout << "Unique values in preview: " << previewUniqueSet.size() << "/" << show << endl;
    }
    
    cout << "=================================" << endl;
    
    return arr;
}

vector<int> generateLargeRandom(int size = 10000, int minVal = 1, int maxVal = 1000000, 
                               bool printInfo = true, int printLimit = 10) {     
    // Parameter validation     
    if (size <= 0) {         
        throw invalid_argument("Error: Array size must be positive. Got: " + to_string(size));     
    }          

    // Check minVal and maxVal     
    if (minVal > maxVal) {         
        throw invalid_argument("Error: minVal must be less than or equal to maxVal. Got: minVal=" +                               
                              to_string(minVal) + ", maxVal=" + to_string(maxVal));     
    }          

    // Initialize random seed     
    static bool seeded = false;     
    if (!seeded) {         
        srand(time(nullptr));         
        seeded = true;     
    }          

    // Generate array     
    vector<int> arr;     
    try {         
        // Pre-allocate memory for better performance with large arrays         
        arr.reserve(size);                  

        // Calculate random number range         
        long long range = static_cast<long long>(maxVal) - minVal + 1;                  

        // For large ranges, use better random number generation         
        if (range > RAND_MAX) {             
            // Use multiple rand() calls to generate larger random numbers             
            for (int i = 0; i < size; i++) {                 
                // Generate random number between 0 and range-1                 
                long long random_value = 0;                 
                // Combine multiple rand() values for larger range                 
                int parts = (range + RAND_MAX) / RAND_MAX;                 
                for (int j = 0; j < parts; j++) {                     
                    random_value = random_value * (RAND_MAX + 1LL) + rand();                 
                }                 
                arr.push_back(minVal + static_cast<int>(random_value % range));             
            }         
        } else {             
            // Small range, use rand() directly             
            for (int i = 0; i < size; i++) {                 
                arr.push_back(minVal + rand() % static_cast<int>(range));             
            }         
        }     
    } catch (const bad_alloc& e) {         
        // Memory allocation failure         
        cerr << "Memory allocation failed for size " << size << ": " << e.what() << endl;         
        throw;     
    } catch (...) {         
        // Other exceptions         
        cerr << "Unknown error occurred while generating array of size " << size << endl;         
        throw;     
    }          

    if (!printInfo) {
        return arr;
    }
    
    DatasetFeatures features;
    features.data = arr;
    features.size = size;
    
    // Set dataset type
    if (size >= 10000) {
        features.type = "Large Random Array [" + to_string(minVal) + ".." + to_string(maxVal) + "]";
    } else if (size >= 1000) {
        features.type = "Medium Random Array [" + to_string(minVal) + ".." + to_string(maxVal) + "]";
    } else {
        features.type = "Small Random Array [" + to_string(minVal) + ".." + to_string(maxVal) + "]";
    }
    
    // Calculate sortedness (sample-based for large arrays)
    if (arr.size() < 2) {
        features.sortedness = 0.5;
    } else {
        int checkSize = min(1000, size);
        long long inversions = 0;
        
        for (int i = 0; i < checkSize; i++) {
            for (int j = i + 1; j < checkSize; j++) {
                if (arr[i] > arr[j]) inversions++;
            }
        }
        
        long long maxInversions = static_cast<long long>(checkSize) * (checkSize - 1) / 2;
        if (maxInversions == 0) {
            features.sortedness = 0.5;
        } else {
            double inversionRatio = static_cast<double>(inversions) / maxInversions;
            features.sortedness = 1.0 - 2.0 * fabs(inversionRatio - 0.5);
            features.sortedness = max(0.0, min(1.0, features.sortedness));
        }
    }
    
    // Calculate unique count (sample-based for large arrays)
    if (size <= 5000) {
        unordered_set<int> uniqueSet(arr.begin(), arr.end());
        features.uniqueCount = uniqueSet.size();
    } else {
        int checkSize = min(5000, size);
        unordered_set<int> uniqueSet(arr.begin(), arr.begin() + checkSize);
        features.uniqueCount = uniqueSet.size();
    }
    cout << "=== Generated Random Array ===" << endl;
    cout << "Type: " << features.type << endl;
    cout << "Size: " << features.size << " elements" << endl;
    cout << "Value range: [" << minVal << ".." << maxVal << "]" << endl;
    cout << "Sortedness: " << fixed << setprecision(3) << features.sortedness 
         << " (0=random, 1=sorted)" << endl;
    cout << "Unique values (in checked portion): " << features.uniqueCount << endl;
    
    if (size > 0) {
        int checkSize = min(1000, size);
        int minValActual = arr[0];
        int maxValActual = arr[0];
        long long sum = 0;
        
        for (int i = 0; i < checkSize; i++) {
            if (arr[i] < minValActual) minValActual = arr[i];
            if (arr[i] > maxValActual) maxValActual = arr[i];
            sum += arr[i];
        }
        
        cout << "Checked range: [" << minValActual << ".." << maxValActual << "]" << endl;
        cout << "Checked average: " << fixed << setprecision(2) 
             << (static_cast<double>(sum) / checkSize) << endl;
    }
    
    int show = min(printLimit, size);
    cout << "Array preview [" << show << "/" << size << "]: [";
    for (int i = 0; i < show; i++) {
        cout << arr[i];
        if (i < show - 1) cout << ", ";
    }
    if (size > show) {
        cout << ", ... (" << size - show << " more)";
    }
    cout << "]" << endl;
    
    return arr;
}

// Task 2 & 3: Sorting Algorithms with comparison counting
void bubbleSort(vector<int>& arr, long long& comparisons);
void insertionSort(vector<int>& arr, long long& comparisons);
void mergeSort(vector<int>& arr, int l, int r, long long& comparisons);
void quickSort(vector<int>& arr, int low, int high, long long& comparisons);

// Task 4: AI Module
DatasetFeatures analyzeDataset(const vector<int>& data);
AlgoType predictBestAlgorithm(const DatasetFeatures& features);
string getAlgoName(AlgoType type);

void bubbleSort(vector<int>& arr, long long& comparisons) {
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
        if (!swapped) break;  // Early termination if sorted
    }
}

void insertionSort(vector<int>& arr, long long& comparisons) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        // Count comparisons only once per shift
        while (j >= 0) {
            comparisons++;
            if (arr[j] <= key) break;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void merge(vector<int>& arr, int l, int m, int r, long long& comparisons) {
    int n1 = m - l + 1;
    int n2 = r - m;
    
    vector<int> left(n1);
    vector<int> right(n2);
    
    for (int i = 0; i < n1; i++)
        left[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        right[j] = arr[m + 1 + j];
    
    int i = 0, j = 0, k = l;
    
    while (i < n1 && j < n2) {
        comparisons++;
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        arr[k] = left[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = right[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& arr, int l, int r, long long& comparisons) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m, comparisons);
    mergeSort(arr, m + 1, r, comparisons);
    merge(arr, l, m, r, comparisons);
}

int partition(vector<int>& arr, int low, int high, long long& comparisons) {
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

void quickSort(vector<int>& arr, int low, int high, long long& comparisons) {
    if (low < high) {
        int pi = partition(arr, low, high, comparisons);
        quickSort(arr, low, pi - 1, comparisons);
        quickSort(arr, pi + 1, high, comparisons);
    }
}

DatasetFeatures analyzeDataset(const vector<int>& data) {
    DatasetFeatures features;
    features.size = data.size();
    features.isLargeDataset = (features.size > 1000);
    
    if (features.size <= 1) {
        features.sortedness = 1.0;
        features.reversedness = 0.0;
        features.uniqueCount = features.size;
        features.uniqueRatio = 1.0;
        features.type = "Single Element";
        return features;
    }
    
    // Analyze sortedness and reversedness
    long long ascendingPairs = 0;
    long long descendingPairs = 0;
    
    for (int i = 0; i < features.size - 1; i++) {
        if (data[i] <= data[i+1]) ascendingPairs++;
        if (data[i] >= data[i+1]) descendingPairs++;
    }
    
    features.sortedness = (double)ascendingPairs / (features.size - 1);
    features.reversedness = (double)descendingPairs / (features.size - 1);
    
    // Analyze uniqueness (sample first 1000 elements for performance)
    int sampleSize = min(1000, features.size);
    unordered_set<int> uniqueElements;
    for (int i = 0; i < sampleSize; i++) {
        uniqueElements.insert(data[i]);
    }
    
    features.uniqueCount = uniqueElements.size();
    features.uniqueRatio = (double)uniqueElements.size() / sampleSize;
    
    if (features.sortedness >= 0.90) {
        features.type = "Nearly Sorted";
    } else if (features.reversedness >= 0.90) {
        features.type = "Reversed";
    } else if (features.uniqueRatio < 0.40) {
        features.type = "Few Unique";
    } else if (features.isLargeDataset) {
        features.type = "Large Random";
    } else {
        features.type = "Random";
    }
    
    return features;
}

AlgoType predictBestAlgorithm(const DatasetFeatures& features) {
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
    // Insertion Sort worst case O(N^2) for reversed data
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

string getAlgoName(AlgoType type) {
    switch (type) {
        case BUBBLE_SORT: return "Bubble Sort";
        case INSERTION_SORT: return "Insertion Sort";
        case MERGE_SORT: return "Merge Sort";
        case QUICK_SORT: return "Quick Sort";
        default: return "Unknown";
    }
}

SortMetrics runSort(AlgoType type, vector<int> data) {
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

int main() {
    srand(time(0));
    
    int choice, size;
    vector<int> originalData;

    cout << "=== AI-Driven Sorting Algorithm Optimizer ===\n";
    cout << "Select Dataset Type:\n";
    cout << "1. Random\n2. Nearly Sorted\n3. Reversed\n4. Few Unique\n";
    cout << "Enter choice: ";
    cin >> choice;
    
    cout << "Enter Dataset Size: ";
    cin >> size;

    switch(choice) {
        case 1: originalData = generateRandomDataset(size, 1, size * 10, false); break;
        case 2: originalData = generateNearlySorted(size, 0.1, false); break;
        case 3: originalData = generateReversed(size, 10); break;
        case 4: originalData = generateFewUniqueArray(size, min(5, size), false); break;
        default: originalData = generateRandomDataset(size, 1, size * 10, false);
    }
    
    if (size <= 50) {
        cout << "Original Data: ";
        for (int x : originalData) cout << x << " ";
        cout << "\n";
    }

    DatasetFeatures features = analyzeDataset(originalData);
    cout << "\n[AI Analysis Report]" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "Dataset Characteristics:" << endl;
    cout << "  > Size:        " << features.size << (features.isLargeDataset ? " (Large)" : " (Small/Medium)") << endl;
    cout << "  > Type:        " << features.type << endl;
    cout << "  > Sortedness:  " << fixed << setprecision(1) << (features.sortedness * 100.0) << "%" << endl;
    cout << "  > Reversed:    " << (features.reversedness * 100.0) << "%" << endl;
    cout << "  > Uniqueness:  " << (features.uniqueRatio * 100.0) << "% (Estimated from sample)" << endl;
    cout << "  > Unique Count: " << features.uniqueCount << endl;
    cout << "------------------------------------------------" << endl;
    
    AlgoType predicted = predictBestAlgorithm(features);
    cout << ">>> AI Predicts Best Algorithm: " << getAlgoName(predicted) << " <<<\n\n";

    vector<SortMetrics> results;
    
    if (size <= 1000) {
        results.push_back(runSort(BUBBLE_SORT, originalData));
        results.push_back(runSort(INSERTION_SORT, originalData));
    } else {
        cout << "(Skipping O(n^2) algorithms due to large size)\n";
    }
    
    results.push_back(runSort(MERGE_SORT, originalData));
    results.push_back(runSort(QUICK_SORT, originalData));

    cout << left << setw(20) << "Algorithm";
    cout << setw(15) << "Comparisons";
    cout << setw(15) << "Time (ms)" << "\n";
    cout << "--------------------------------------------------\n";
    
    string actualBestAlgo;
    double minTime = 1e9;

    for (const auto& res : results) {
        cout << left << setw(20) << res.algoName;
        cout << setw(15) << res.comparisons;
        cout << setw(15) << res.executionTimeMs << "\n";
        
        if (res.executionTimeMs < minTime) {
            minTime = res.executionTimeMs;
            actualBestAlgo = res.algoName;
        }
    }
    
    cout << "\nActual Best Algorithm: " << actualBestAlgo << "\n";
    if (getAlgoName(predicted) == actualBestAlgo) {
        cout << "AI Prediction was CORRECT!\n";
    } else {
        cout << "AI Prediction was INCORRECT.\n";
    }

    return 0;
}
