// AI_Optimizer.h
// 对应课程: CST207 Design and Analysis of Algorithms
// 模块功能: 负责分析数据集特征并推荐最佳排序算法

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
// 包含所有决策所需的量化指标
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
    // 内部使用的阈值常量 (便于调优)
    static const int SIZE_THRESHOLD_SMALL = 50;     // 小数据集阈值
    static const int SIZE_THRESHOLD_LARGE = 1000;   // 大数据集阈值 
    static constexpr double SORTED_THRESHOLD = 0.90; // 有序判定阈值
    static constexpr double UNIQUE_THRESHOLD = 0.40; // 重复判定阈值
};

#endif // AI_OPTIMIZER_H