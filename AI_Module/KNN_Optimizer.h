#ifndef KNN_OPTIMIZER_H
#define KNN_OPTIMIZER_H

#include <vector>
#include <string>
#include <cmath>

enum AlgorithmType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

// 特征结构体
struct DatasetFeatures {
    int size;               
    double sortedness;      // 0.0 - 1.0
    double reversedness;    // 0.0 - 1.0
    double uniqueness;      // 0.0 - 1.0
};

// 训练样本结构
struct TrainingSample {
    DatasetFeatures features;
    AlgorithmType bestAlgo;
};

class KNNOptimizer {
public:
    KNNOptimizer(); // 构造函数初始化知识库

    // 核心功能：提取特征 (O(N) 采样优化)
    static DatasetFeatures extractFeatures(int* arr, int n);

    // 核心功能：预测 (使用加权 k-NN)
    AlgorithmType predict(DatasetFeatures input, int k = 5);

    // 辅助功能：获取名称
    static std::string getAlgorithmName(AlgorithmType type);

private:
    std::vector<TrainingSample> trainingData;

    // 计算加权欧几里得距离
    double calculateDistance(const DatasetFeatures& f1, const DatasetFeatures& f2);

    // 归一化辅助函数 (将 Size 映射到 0-1)
    double normalizeSize(int size);

    // 安全守卫：防止在大数据集上运行 O(N^2) 算法
    AlgorithmType enforceSafetyRules(AlgorithmType predicted, int dataSize);
};

#endif