// AI_Optimizer.h
// Course: CST207 Design and Analysis of Algorithms
// Module: AI-Driven Sorting Algorithm Optimizer

#ifndef AI_OPTIMIZER_H
#define AI_OPTIMIZER_H

#include <string>
#include <vector>

// 定义支持的排序算法枚举
enum AlgorithmType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

// 定义数据集特征结构体
// 包含 AI 决策所需的量化指标
struct DatasetFeatures {
    int size;                   // 数据规模
    double sortednessRatio;     // 有序度 (0.0 ~ 1.0)
    double reversedRatio;       // 逆序度 (0.0 ~ 1.0)
    double uniqueRatio;         // 唯一性 (0.0 ~ 1.0, 估算值)
    bool isLargeDataset;        // 是否为大数据集 (>1000)
};

class AIOptimizer {
public:
    // 核心功能 1: 提取特征
    // 输入: 数组指针和大小
    // 输出: 特征结构体
    static DatasetFeatures analyzeDataset(int* arr, int n);

    // 核心功能 2: 预测最佳算法 (决策树逻辑)
    // 输入: 特征结构体
    // 输出: 推荐的算法类型
    static AlgorithmType predict(DatasetFeatures features);

    // 辅助功能: 获取算法名称字符串
    static std::string getAlgorithmName(AlgorithmType type);

    // 辅助功能: 打印详细的分析报告 (用于控制台展示)
    static void printAnalysisReport(DatasetFeatures features, AlgorithmType recommendation);
    
private:
    // --- 决策树参数调优 (Decision Tree Hyperparameters) ---
    
    // 1. 小数据集阈值: 128
    // 依据: 现代 CPU 缓存优化使得插入排序在 <128 时通常快于递归算法(Quick/Merge)
    static const int SIZE_THRESHOLD_SMALL = 128;     
    
    // 2. 大数据集阈值: 1000
    // 依据: 项目文档要求，超过 1000 的数据应跳过 O(N^2) 算法
    static const int SIZE_THRESHOLD_LARGE = 1000;   
    
    // 3. 有序判定阈值: 0.95
    // 依据: 只有非常接近有序时，插入排序的 O(N) 优势才能抵消其最坏情况风险
    static constexpr double SORTED_THRESHOLD = 0.95; 

    // 4. 逆序判定阈值: 0.80 (根据组长建议调整)
    // 依据: 只要有 80% 是逆序，QuickSort 性能风险极高，必须强制切换到 MergeSort
    static constexpr double REVERSED_THRESHOLD = 0.80;

    // 5. 重复率判定阈值: 0.40
    // 依据: 当唯一元素少于 40% 时，归并排序处理重复值的稳定性优于快排
    static constexpr double UNIQUE_THRESHOLD = 0.40; 
};

#endif // AI_OPTIMIZER_H
