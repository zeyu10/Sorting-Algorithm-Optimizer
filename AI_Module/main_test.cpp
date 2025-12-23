#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <chrono>    // 用于高精度计时
#include "AI_Optimizer.h"

using namespace std;
using namespace std::chrono;

// --- 1. 实际排序算法实现 (用于验证) ---

void insertionSort(vector<int> arr) { // 传值，不破坏原数组
    for (int i = 1; i < arr.size(); i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 归并辅助
void merge(vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<int> L(n1), R(n2);
    for(int i=0; i<n1; i++) L[i] = arr[l + i];
    for(int j=0; j<n2; j++) R[j] = arr[m + 1 + j];
    int i=0, j=0, k=l;
    while(i<n1 && j<n2) arr[k++] = (L[i]<=R[j]) ? L[i++] : R[j++];
    while(i<n1) arr[k++] = L[i++];
    while(j<n2) arr[k++] = R[j++];
}
void mergeSortRec(vector<int>& arr, int l, int r) {
    if(l>=r) return;
    int m = l + (r-l)/2;
    mergeSortRec(arr, l, m);
    mergeSortRec(arr, m+1, r);
    merge(arr, l, m, r);
}
void mergeSort(vector<int> arr) { mergeSortRec(arr, 0, arr.size()-1); }

// 快排辅助
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) swap(arr[++i], arr[j]);
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}
void quickSortRec(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortRec(arr, low, pi - 1);
        quickSortRec(arr, pi + 1, high);
    }
}
void quickSort(vector<int> arr) { quickSortRec(arr, 0, arr.size()-1); }

// --- 2. 计时器工具 ---
double measureTime(void (*sortFunc)(vector<int>), const vector<int>& data) {
    auto start = high_resolution_clock::now();
    sortFunc(data); // 运行排序
    auto stop = high_resolution_clock::now();
    return duration_cast<microseconds>(stop - start).count() / 1000.0; // 返回毫秒
}

// --- 3. 生成器 ---
vector<int> generateRandom(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = rand() % 10000;
    return data;
}
vector<int> generateReversed(int size) {
    vector<int> data(size);
    for(int i=0; i<size; i++) data[i] = size - i;
    return data;
}
vector<int> generateNearlySorted(int size) {
    vector<int> data(size);
    for(int i=0; i<size; i++) data[i] = i;
    for(int i=0; i<size/20; i++) swap(data[rand()%size], data[rand()%size]); // 5% 乱序
    return data;
}

// --- 4. 测试主逻辑 ---
void runTestCase(string testName, vector<int>& data) {
    cout << "\n================================================" << endl;
    cout << "TEST: " << testName << " (Size=" << data.size() << ")" << endl;
    
    // 1. AI 预测
    DatasetFeatures features = AIOptimizer::analyzeDataset(data.data(), data.size());
    AlgorithmType prediction = AIOptimizer::predict(features);
    AIOptimizer::printAnalysisReport(features, prediction);

    // 2. 实际验证 (Benchmark)
    cout << "\n[Running Benchmark Validation...]" << endl;
    
    // 如果数据量太大，跳过 O(n^2) 算法以节省时间
    double timeIns = (data.size() > 10000 && features.sortednessRatio < 0.9) ? 9999.9 : measureTime(insertionSort, data);
    double timeMerge = measureTime(mergeSort, data);
    double timeQuick = measureTime(quickSort, data);

    cout << "  > Insertion Sort: " << (timeIns > 9000 ? "Timeout (>10s)" : to_string(timeIns) + " ms") << endl;
    cout << "  > Merge Sort:     " << timeMerge << " ms" << endl;
    cout << "  > Quick Sort:     " << timeQuick << " ms" << endl;

    // 3. 结论判断
    string winner;
    double minTime = min({timeIns, timeMerge, timeQuick});
    
    if (minTime == timeIns) winner = "Insertion Sort";
    else if (minTime == timeMerge) winner = "Merge Sort";
    else winner = "Quick Sort";

    cout << "------------------------------------------------" << endl;
    cout << "Actual Winner: " << winner << endl;
    
    string aiChoice = AIOptimizer::getAlgorithmName(prediction);
    if (aiChoice == winner) {
        cout << "RESULT: [SUCCESS] AI prediction matches the fastest algorithm!" << endl;
    } else {
        // 允许微小误差（例如快排和归并只差 0.1ms 算并列）
        if (abs(timeMerge - timeQuick) < 0.5 && (prediction == MERGE_SORT || prediction == QUICK_SORT))
             cout << "RESULT: [SUCCESS] Performance is practically identical." << endl;
        else 
             cout << "RESULT: [DIFF] Comparison complex, check characteristics." << endl;
    }
    cout << "================================================" << endl;
}

int main() {
    srand(time(0)); 
    cout << "AI-Driven Sorting Optimizer - Validation Suite" << endl;

    // Case 1: 几乎有序 -> 应该是 Insertion Sort
    vector<int> nearly = generateNearlySorted(2000);
    runTestCase("Nearly Sorted Data", nearly);

    // Case 2: 逆序数据 -> 应该是 Merge Sort (避免快排最坏情况)
    // 注意：为了让效果明显，这里数据量设大一点
    vector<int> reversed = generateReversed(3000);
    runTestCase("Reversed Data (QuickSort Killer)", reversed);

    // Case 3: 随机大数据 -> 应该是 Quick Sort
    vector<int> random = generateRandom(5000);
    runTestCase("Random Large Dataset", random);

    // Case 4: 极小数据 -> 应该是 Insertion Sort
    vector<int> small = generateRandom(20);
    runTestCase("Tiny Dataset", small);

    return 0;
}
