#ifndef KNN_OPTIMIZER_H
#define KNN_OPTIMIZER_H

#include <vector>
#include <string>
#include <cmath>

// 保持和之前一样的枚举
enum AlgorithmType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

// 单个训练样本的结构
struct TrainingSample {
    double normSize;        // 归一化后的大小 (Size / 10000.0)
    double sortedness;      // 0-1
    double reversedness;    // 0-1
    double uniqueness;      // 0-1
    AlgorithmType bestAlgo; // 这个特征组合下的最佳算法
};

// 数据集特征 (输入)
struct DatasetFeatures {
    int originalSize;
    double sortedness;
    double reversedness;
    double uniqueness;
};

class KNNOptimizer {
public:
    // 初始化：加载“伪造”的训练数据
    KNNOptimizer(); 

    // 核心功能：预测
    // k = 3 (默认找最近的3个邻居)
    AlgorithmType predict(DatasetFeatures features, int k = 3);

    // 辅助：提取特征 (和之前一样，但为了完整性我们可以复用或重写)
    static DatasetFeatures extractFeatures(int* arr, int n);
    
    // 辅助：获取名字
    static std::string getAlgorithmName(AlgorithmType type);

private:
    std::vector<TrainingSample> trainingData;

    // 计算两个点之间的欧几里得距离
    double calculateDistance(const TrainingSample& sample, const DatasetFeatures& input);
};

#endif