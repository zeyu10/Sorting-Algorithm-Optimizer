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
    double sortedness;  // 0.0（乱序）到 1.0（已排序）
    int uniqueCount;    // 唯一元素数量
    string type;        // 数组类型   
};

struct SortMetrics {
    long long comparisons = 0; // 比较次数
    double executionTimeMs = 0.0; // 执行时间（毫秒）
    string algoName;
};

// Task 1: Dataset Generation
vector<int> generateRandomDataset(int size,
                             int minVal = numeric_limits<int>::min(), 
                             int maxVal = numeric_limits<int>::max()) {
    // Strict parameter checking
    if (size <= 0) {
        throw invalid_argument("Array size must be positive");
    }
    
    if (minVal > maxVal) {
        throw invalid_argument("Minimum value cannot be greater than maximum value");
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
    
    return arr;
}
vector<int> generateNearlySorted(int size,float disorderPercent = 0.1) {
    if (size <= 0) return vector<int>();
    
    // Limit disorder percentage between 0-50%
    if (disorderPercent < 0) disorderPercent = 0;
    if (disorderPercent > 0.5) disorderPercent = 0.5;
    
    // Initialize random seed
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    // 1. Create sorted array
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;
    }
    
    // 2. Calculate number of elements to disorder
    int disorderCount = static_cast<int>(size * disorderPercent);
    
    // 3. Randomly select positions and replace with random values
    for (int i = 0; i < disorderCount; i++) {
        int randomIndex = rand() % size;
        int randomValue = 1 + rand() % (size * 2);  // Larger range
        arr[randomIndex] = randomValue;
    }
    
    return arr;
}
DatasetFeatures generateReversed(int size, int printLimit = 10) {     
    DatasetFeatures result;
    
    if (size <= 0) {         
        cerr << "Error: Array size must be positive" << endl;         
        result.type = "Error";
        return result;
    }          
    
    vector<int> arr(size);
    
    // 使用当前时间作为随机种子
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(1, size * 10);
    
    // 生成随机数组
    for (int i = 0; i < size; i++) {
        arr[i] = distribution(generator);
    }
    
    // 按降序排序
    sort(arr.begin(), arr.end(), greater<int>());
    
    // 计算特征
    result.data = arr;
    result.size = size;
    result.type = "Random descending array";
    
    // 计算有序度
    if (size < 2) {
        result.sortedness = 1.0;
    } else {
        // 降序数组的有序度接近1.0
        int correct = 0;
        int totalPairs = size * (size - 1) / 2;
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                if (arr[i] >= arr[j]) correct++;
            }
        }
        result.sortedness = (double)correct / totalPairs;
    }
    
    // 计算唯一元素数量
    unordered_set<int> uniqueSet(arr.begin(), arr.end());
    result.uniqueCount = uniqueSet.size();
    
    // 打印数组
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
    
    return result; 
}
vector<int> generateFewUniqueArray(int size, int uniqueCount = 5, 
                                  bool printArray = true, 
                                  int printLimit = 10) {
    // Strict parameter checking - all errors throw exceptions
    
    if (size <= 0) {
        throw invalid_argument("Array size must be positive. Got: " + to_string(size));
    }
    
    if (uniqueCount <= 0) {
        throw invalid_argument("Unique count must be positive. Got: " + to_string(uniqueCount));
    }
    
    // 如果唯一值数量大于数组大小，报错而不是调整
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
    
    // 1. Generate unique values set (random values)
    unordered_set<int> uniqueSet;
    vector<int> uniqueValues;
    
    // 确保生成指定数量的唯一值
    // 使用足够大的范围以确保能生成足够的唯一值
    int maxRandomValue = 1000000; // 足够大的范围
    
    for (int i = 0; i < uniqueCount; i++) {
        // 尝试生成唯一值
        int attempts = 0;
        const int MAX_ATTEMPTS = 1000;
        bool valueAdded = false;
        
        while (attempts < MAX_ATTEMPTS && !valueAdded) {
            // 生成1到maxRandomValue之间的随机值
            int randomValue = rand() % maxRandomValue + 1;
            if (uniqueSet.find(randomValue) == uniqueSet.end()) {
                uniqueSet.insert(randomValue);
                uniqueValues.push_back(randomValue);
                valueAdded = true;
            }
            attempts++;
        }
        
        // 如果多次尝试后仍无法生成唯一值，使用递增策略
        if (!valueAdded) {
            // 使用基础值加上偏移量来确保唯一性
            int baseValue = 10000 * (i + 1);
            int offset = rand() % 1000;
            int fallbackValue = baseValue + offset;
            
            // 确保fallbackValue是唯一的
            while (uniqueSet.find(fallbackValue) != uniqueSet.end()) {
                fallbackValue++;
            }
            
            uniqueSet.insert(fallbackValue);
            uniqueValues.push_back(fallbackValue);
        }
    }
    
    // 2. Fill array with these unique values (随机分布)
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        int randomIndex = rand() % uniqueValues.size();
        arr[i] = uniqueValues[randomIndex];
    }
    
    // 3. Print the array if requested
    if (printArray) {
        cout << "Generated few-unique array:" << endl;
        cout << "  Size: " << size << " elements" << endl;
        
        // 统计数组中实际有多少个不同的值
        unordered_set<int> actualUniqueSet(arr.begin(), arr.end());
        int actualUniqueCount = actualUniqueSet.size();
        cout << "  Unique values in array: " << actualUniqueCount 
             << " (requested: " << uniqueCount << ")" << endl;
        
        if (actualUniqueCount < uniqueCount) {
            cout << "  Note: Not all " << uniqueCount 
                 << " generated values appear in the full array" << endl;
        }
        
        // 显示生成的所有唯一值（排序后）
        sort(uniqueValues.begin(), uniqueValues.end());
        cout << "  Generated unique values: [";
        
        size_t showUnique = min(uniqueValues.size(), (size_t)5);
        for (size_t i = 0; i < showUnique; i++) {
            cout << uniqueValues[i];
            if (i < showUnique - 1) cout << ", ";
        }
        if (uniqueValues.size() > 5) {
            cout << ", ... (" << uniqueValues.size() - 5 << " more)";
        }
        cout << "]" << endl;
        
        // 显示数组内容
        int show = min(size, printLimit);
        cout << "  Array preview [" << show << "/" << size << "]: [";
        
        for (int i = 0; i < show; i++) {
            cout << arr[i];
            if (i < show - 1) cout << ", ";
        }
        
        if (size > printLimit) {
            cout << ", ... (" << size - printLimit << " more)";
        }
        cout << "]" << endl;
        
        // 显示预览部分的分布统计
        if (show >= 3) {
            unordered_map<int, int> freq;
            for (int i = 0; i < show; i++) {
                freq[arr[i]]++;
            }
            
            if (freq.size() <= 10) { // 如果唯一值不多，显示完整统计
                cout << "  Distribution in preview: ";
                vector<pair<int, int>> freqVec(freq.begin(), freq.end());
                sort(freqVec.begin(), freqVec.end(), 
                     [](const pair<int, int>& a, const pair<int, int>& b) {
                         return a.second > b.second; // 按频率降序
                     });
                
                for (size_t i = 0; i < freqVec.size(); i++) {
                    if (i > 0) cout << ", ";
                    cout << freqVec[i].first << ":" << freqVec[i].second;
                }
                cout << endl;
            }
        }
    }
    
    return arr;
}
vector<int> generateLargeRandom(int size = 10000, int minVal = 1, int maxVal = 1000000) {
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
    
    return arr;
}

// Task 2 & 3: Sorting Algorithms（注意：都需要引用传递 comparisons 以计数）
void bubbleSort(vector<int>& arr, long long& comparisons);
void insertionSort(vector<int>& arr, long long& comparisons);
void mergeSort(vector<int>& arr, int l, int r, long long& comparisons);
void quickSort(vector<int>& arr, int low, int high, long long& comparisons);

// Task 4: AI Module
DatasetFeatures analyzeDataset(const vector<int>& data);
AlgoType predictBestAlgorithm(const DatasetFeatures& features);
string getAlgoName(AlgoType type);

vector<int> generateRandom(int size) {
    vector<int> data;
    // TODO: 生成完全随机的数组
    return data;
}

vector<int> generateNearlySorted(int size) {
    vector<int> data;
    // TODO: 生成几乎有序的数组（先排序，再随机交换少量元素）
    return data;
}

vector<int> generateReversed(int size) {
    vector<int> data;
    // TODO: 生成降序排列的数组
    return data;
}

vector<int> generateFewUnique(int size) {
    vector<int> data;
    // TODO: 生成含有大量重复元素的数组
    return data;
}

vector<int> generateLargeRandom(int size) {
    // TODO: 生成大容量随机数组（可以是 generateRandom 的别名）
    return generateRandom(size);
}

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
    // TODO: Merge Sort 的合并逻辑
}

void mergeSort(vector<int>& arr, int l, int r, long long& comparisons) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m, comparisons);
    mergeSort(arr, m + 1, r, comparisons);
    merge(arr, l, m, r, comparisons);
}

int partition(vector<int>& arr, int low, int high, long long& comparisons) {
    // TODO: Quick Sort 的 Partition 逻辑
    return 0; // 替换为 pivot index
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
    
    // TODO: 计算 sortedness（例如：计算逆序对数量，或者相邻有序元素的比例）
    features.sortedness = 0.0; 
    
    // TODO: 计算 uniqueCount（可以使用 set 或排序后去重计数）
    features.uniqueCount = 0;
    
    return features;
}

AlgoType predictBestAlgorithm(const DatasetFeatures& features) {
    // TODO: 根据 features 返回推荐算法
    // 可以使用 Decision Tree 逻辑 (if-else)
    // 例如: if (features.size < 100) return INSERTION_SORT;
    // else if (features.sortedness > 0.9) return INSERTION_SORT;
    // else return QUICK_SORT;
    
    return QUICK_SORT; // 默认返回
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
        case 1: originalData = generateRandom(size); break;
        case 2: originalData = generateNearlySorted(size); break;
        case 3: originalData = generateReversed(size); break;
        case 4: originalData = generateFewUnique(size); break;
        default: originalData = generateRandom(size);
    }
    
    if (size <= 50) {
        cout << "Original Data: ";
        for (int x : originalData) cout << x << " ";
        cout << "\n";
    }

    DatasetFeatures features = analyzeDataset(originalData);
    cout << "\nAnalyzing Dataset Features...\n";
    cout << "Size: " << features.size << ", Unique: " << features.uniqueCount << "\n";
    
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

    // 4. 展示结果表格
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
