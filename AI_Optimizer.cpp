// AI_Optimizer.cpp
#include "AI_Optimizer.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <cmath>

// 核心功能 1: 提取特征
// 复杂度: O(N) 或 O(Sample Size)，确保分析本身不消耗过多时间
DatasetFeatures AIOptimizer::analyzeDataset(int* arr, int n) {
    DatasetFeatures features;
    features.size = n;
    features.isLargeDataset = (n > SIZE_THRESHOLD_LARGE);

    // 如果数组为空或只有一个元素，直接返回默认特征
    if (n <= 1) {
        features.sortednessRatio = 1.0;
        features.reversedRatio = 0.0;
        features.uniqueRatio = 1.0;
        return features;
    }

    // --- 1. 分析顺序 (Sortedness & Reversed) ---
    // 通过统计相邻元素的升序/降序对来实现
    long long ascendingPairs = 0;
    long long descendingPairs = 0;

    // 为了防止分析过程太慢，对于超大数据集(如 > 100000)，可以考虑步长采样
    // 但为了准确性，这里默认遍历所有相邻对 (O(N) 是可以接受的)
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] <= arr[i+1]) ascendingPairs++;
        if (arr[i] >= arr[i+1]) descendingPairs++;
    }

    features.sortednessRatio = (double)ascendingPairs / (n - 1);
    features.reversedRatio = (double)descendingPairs / (n - 1);

    // --- 2. 分析唯一性 (Uniqueness) ---
    // 针对 "Few Unique" 数据集。为了性能，只采样前 100 个元素进行估算。
    // 使用 std::unordered_set 可以在 O(1) 平均时间内完成插入。
    int sampleSize = (n < 100) ? n : 100;
    std::unordered_set<int> uniqueElements;
    for (int i = 0; i < sampleSize; i++) {
        uniqueElements.insert(arr[i]);
    }
    features.uniqueRatio = (double)uniqueElements.size() / sampleSize;

    return features;
}

// 核心功能 2: 预测最佳算法 (Decision Tree)
// 依据算法的时间复杂度理论进行分支判断
AlgorithmType AIOptimizer::predict(DatasetFeatures f) {
    
    // 规则 1: 极小数据集 (Size < 50)
    // Insertion Sort 尽管是 O(N^2)，但在 N 很小时常数项极低，往往快于 Quick/Merge 的递归开销。
    if (f.size <= SIZE_THRESHOLD_SMALL) {
        return INSERTION_SORT; 
    }

    // 规则 2: 大数据集的安全检查 (Size > 1000) 
    // 文档指出：Arrays larger than 1000 should skip Bubble/Insertion.
    // 因此，如果数据量大，即使是有序的，为了保险起见（避免 Stack Overflow 或超时），
    // 我们也倾向于推荐 O(N log N) 的算法，除非它是 100% 有序。
    if (f.isLargeDataset) {
        // 如果是严格的 Few Unique (重复极多)，Merge Sort 通常比标准 Quick Sort 表现更稳
        if (f.uniqueRatio < UNIQUE_THRESHOLD) {
            return MERGE_SORT;
        }
        // 默认的大数据选择：Quick Sort
        return QUICK_SORT;
    }

    // 规则 3: 中等规模数据集 (50 < Size <= 1000)
    // 这里我们可以灵活运用 O(N) 的特性

    // Case A: 几乎有序 (Nearly Sorted)
    // Insertion Sort 在近乎有序时退化为 O(N)，非常快。
    if (f.sortednessRatio >= SORTED_THRESHOLD) {
        return INSERTION_SORT;
    }

    // Case B: 逆序 (Reversed)
    // 这是一个陷阱！Insertion Sort 在逆序时是最坏情况 O(N^2)。
    // 必须避开 Insertion，使用 Quick Sort。
    if (f.reversedRatio >= SORTED_THRESHOLD) {
        return QUICK_SORT;
    }

    // Case C: 重复元素多 (Few Unique)
    // 推荐 Merge Sort
    if (f.uniqueRatio < UNIQUE_THRESHOLD) {
        return MERGE_SORT;
    }

    // Case D: 随机数据 (Random)
    // Quick Sort 是通常情况下的王者
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
    std::cout << "  > Uniqueness:  " << (f.uniqueRatio * 100.0) << "% (Estimated from sample)" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "AI Reasoning:" << std::endl;

    if (f.size <= SIZE_THRESHOLD_SMALL) {
        std::cout << "  -> Dataset is very small. Overhead of recursive algorithms (Quick/Merge) outweighs benefits." << std::endl;
    } else if (f.isLargeDataset) {
        std::cout << "  -> Large dataset detected (>1000). Skipping O(N^2) algorithms explicitly." << std::endl;
        if (f.uniqueRatio < UNIQUE_THRESHOLD) 
            std::cout << "  -> High duplication detected. Merge Sort selected for stability." << std::endl;
        else
            std::cout << "  -> Random distribution assumed. Quick Sort selected for best average performance." << std::endl;
    } else if (f.sortednessRatio >= SORTED_THRESHOLD) {
        std::cout << "  -> Data is nearly sorted. Insertion Sort will perform at O(N) speed." << std::endl;
    } else {
         std::cout << "  -> Data is random or reversed. O(N log N) algorithm is required." << std::endl;
    }
    
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << ">>> AI PREDICTION: " << getAlgorithmName(recommendation) << " <<<" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}