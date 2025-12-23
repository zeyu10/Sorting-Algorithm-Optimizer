// AI_Optimizer.cpp
#include "AI_Optimizer.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------
// 核心功能 1: 提取特征 (Feature Extraction)
// 复杂度: O(N) 或 O(Sample Size)
// ---------------------------------------------------------
DatasetFeatures AIOptimizer::analyzeDataset(int* arr, int n) {
    DatasetFeatures features;
    features.size = n;
    features.isLargeDataset = (n > SIZE_THRESHOLD_LARGE);

    // 边界情况处理
    if (n <= 1) {
        features.sortednessRatio = 1.0;
        features.reversedRatio = 0.0;
        features.uniqueRatio = 1.0;
        return features;
    }

    // 1. 分析有序度 (Sortedness) 和 逆序度 (Reversedness)
    // 遍历一次数组统计相邻关系
    long long ascendingPairs = 0;
    long long descendingPairs = 0;

    for (int i = 0; i < n - 1; i++) {
        if (arr[i] <= arr[i+1]) ascendingPairs++;
        if (arr[i] >= arr[i+1]) descendingPairs++;
    }

    features.sortednessRatio = (double)ascendingPairs / (n - 1);
    features.reversedRatio = (double)descendingPairs / (n - 1);

   // 2. 分析唯一性 (Uniqueness)
    // 优化：使用随机采样代替前缀采样，避免数据分布不均导致的误判
    int sampleSize = (n < 100) ? n : 100;
    std::unordered_set<int> uniqueElements;
    
    // 如果样本很少，直接遍历
    if (n <= 100) {
        for (int i = 0; i < n; i++) uniqueElements.insert(arr[i]);
    } else {
        // 随机探针采样 (Random Probing)
        // 注意：需确保外部已设置随机种子，或者在此处简单取模
        for (int i = 0; i < sampleSize; i++) {
            // 简单的伪随机跳跃采样，避免依赖外部 srand
            int idx = (i * 997 + 13) % n; 
            uniqueElements.insert(arr[idx]);
        }
    }
    // ... 上面是你刚粘贴进去的随机采样代码 ...
    features.uniqueRatio = (double)uniqueElements.size() / sampleSize;
    
    return features;  // <--- 必须加上这一行！！！
}

// ---------------------------------------------------------
// 核心功能 2: 预测最佳算法 (Decision Tree Prediction)
// ---------------------------------------------------------
AlgorithmType AIOptimizer::predict(DatasetFeatures f) {
    
    // --- 层级 1: 极小数据集 ---
    // 逻辑: 无递归开销，插入排序最快
    if (f.size <= SIZE_THRESHOLD_SMALL) {
        return INSERTION_SORT; 
    }

    // --- 层级 2: 特殊形态优先 (Special Cases) ---
    
    // Case A: 几乎有序 (Nearly Sorted)
    // 逻辑: 插入排序退化为 O(N)，无敌快
    if (f.sortednessRatio >= SORTED_THRESHOLD) {
        return INSERTION_SORT; 
    }

    // Case B: 逆序 (Reversed) - 组长建议的重点检查
    // 逻辑: 逆序对普通 QuickSort 是最坏情况 O(N^2)。
    // 必须使用 Merge Sort (稳定 O(N log N)) 进行防御。
    if (f.reversedRatio >= REVERSED_THRESHOLD) {
        return MERGE_SORT; 
    }

    // Case C: 重复元素多 (Few Unique)
    // 逻辑: Merge Sort 处理重复值效率更高且稳定
    if (f.uniqueRatio < UNIQUE_THRESHOLD) {
        return MERGE_SORT;
    }

    // --- 层级 3: 一般情况 (General Case) ---
    // 逻辑: 剩下的主要是随机乱序数据，Quick Sort 平均最快
    return QUICK_SORT;
}

std::string AIOptimizer::getAlgorithmName(AlgorithmType type) {
    switch (type) {
        case BUBBLE_SORT: return "Bubble Sort";
        case INSERTION_SORT: return "Insertion Sort";
        case MERGE_SORT: return "Merge Sort";
        case QUICK_SORT: return "Quick Sort";
        default: return "Unknown";
    }
}

void AIOptimizer::printAnalysisReport(DatasetFeatures f, AlgorithmType recommendation) {
    std::cout << "\n[AI Analysis Report]" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Dataset Characteristics:" << std::endl;
    std::cout << "  > Size:        " << f.size << (f.isLargeDataset ? " (Large)" : " (Small/Medium)") << std::endl;
    std::cout << "  > Sortedness:  " << (f.sortednessRatio * 100.0) << "%" << std::endl;
    std::cout << "  > Reversed:    " << (f.reversedRatio * 100.0) << "%" << std::endl;
    std::cout << "  > Uniqueness:  " << (f.uniqueRatio * 100.0) << "% (Estimated)" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "AI Reasoning:" << std::endl;

    if (f.size <= SIZE_THRESHOLD_SMALL) {
        std::cout << "  -> Dataset is very small (<128). Insertion Sort overhead is lower than recursive sorts." << std::endl;
    } else if (f.sortednessRatio >= SORTED_THRESHOLD) {
        std::cout << "  -> Data is nearly sorted. Insertion Sort will run in near O(N) time." << std::endl;
    } else if (f.reversedRatio >= REVERSED_THRESHOLD) {
        std::cout << "  -> High reversedness detected (>80%). Selected Merge Sort to avoid Quick Sort worst-case O(N^2)." << std::endl;
    } else if (f.uniqueRatio < UNIQUE_THRESHOLD) {
         std::cout << "  -> High duplication detected. Merge Sort preferred for stability." << std::endl;
    } else {
         std::cout << "  -> Random distribution. Quick Sort is selected for best average performance." << std::endl;
    }
    
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << ">>> AI PREDICTION: " << getAlgorithmName(recommendation) << " <<<" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}
