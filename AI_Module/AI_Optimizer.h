#ifndef AI_OPTIMIZER_H
#define AI_OPTIMIZER_H

#include <string>
#include <vector>

enum AlgorithmType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

struct DatasetFeatures {
    int size;
    double sortednessRatio;
    double reversedRatio;
    double uniqueRatio;
    bool isLargeDataset;
};

class AIOptimizer {
public:
    static DatasetFeatures analyzeDataset(int* arr, int n);
    static AlgorithmType predict(DatasetFeatures features);
    static std::string getAlgorithmName(AlgorithmType type);
    static void printAnalysisReport(DatasetFeatures features, AlgorithmType recommendation);
    
private:
    // 修改点：统一定义为 constexpr，确保编译期常量，避免链接错误
    static constexpr int SIZE_THRESHOLD_SMALL = 128;      
    static constexpr int SIZE_THRESHOLD_LARGE = 1000;    
    static constexpr double SORTED_THRESHOLD = 0.95;  
    static constexpr double REVERSED_THRESHOLD = 0.80;
    static constexpr double UNIQUE_THRESHOLD = 0.40;  
};

#endif // AI_OPTIMIZER_H
