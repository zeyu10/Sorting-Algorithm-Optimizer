// main_test.cpp (适配 AIOptimizer 决策树版本)
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include "AI_Optimizer.h"

// --- 辅助数据生成 ---
std::vector<int> generateRandom(int size) {
    std::vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = rand() % 10000;
    return data;
}

std::vector<int> generateReversed(int size) {
    std::vector<int> data(size);
    for(int i=0; i<size; i++) data[i]=size-i;
    return data;
}

// --- 测试主逻辑 ---
void runTestCase(std::string testName, std::vector<int>& data) {
    std::cout << "\n------------------------------------------------" << std::endl;
    std::cout << "TEST: " << testName << " (N=" << data.size() << ")" << std::endl;
    
    // 1. 提取特征 (使用 AIOptimizer 类)
    DatasetFeatures features = AIOptimizer::analyzeDataset(data.data(), data.size());
    
    // 2. 预测
    AlgorithmType prediction = AIOptimizer::predict(features);

    // 3. 打印报告
    AIOptimizer::printAnalysisReport(features, prediction);
}

int main() {
    srand(time(0)); 
    std::cout << "Starting Unit Tests (Decision Tree Version)...\n" << std::endl;

    // 关键测试 1: 逆序数据 (Reversed) -> 应该输出 Merge Sort
    std::vector<int> reversedData = generateReversed(2000);
    runTestCase("Large Reversed Dataset", reversedData);

    // 关键测试 2: 小数据 (Small) -> 应该输出 Insertion Sort
    std::vector<int> smallData = generateRandom(50);
    runTestCase("Small Random Dataset", smallData);

    // 关键测试 3: 大随机数据 -> 应该输出 Quick Sort
    std::vector<int> largeRandom = generateRandom(5000);
    runTestCase("Large Random Dataset", largeRandom);

    return 0;
}
