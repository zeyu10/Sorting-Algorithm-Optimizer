#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "AI_Optimizer.h"

// --- 辅助函数：用于生成特定类型的测试数据 ---

// 1. 生成随机数据
std::vector<int> generateRandom(int size) {
    std::vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = rand() % 10000;
    return data;
}

// 2. 生成几乎有序的数据 (Nearly Sorted)
std::vector<int> generateNearlySorted(int size) {
    std::vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = i; // 先完全排序
    
    // 随机交换约 5% 的元素，制造"稍微无序"
    int swaps = size * 0.05; 
    for (int i = 0; i < swaps; i++) {
        int idx1 = rand() % size;
        int idx2 = rand() % size;
        std::swap(data[idx1], data[idx2]);
    }
    return data;
}

// 3. 生成逆序数据 (Reversed)
std::vector<int> generateReversed(int size) {
    std::vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = size - i;
    return data;
}

// 4. 生成大量重复数据 (Few Unique)
std::vector<int> generateFewUnique(int size) {
    std::vector<int> data(size);
    // 只用 10 个不同的数字填充整个数组
    for (int i = 0; i < size; i++) data[i] = rand() % 10; 
    return data;
}

// --- 测试执行函数 ---
void runTestCase(std::string testName, std::vector<int>& data) {
    std::cout << "\n=============================================" << std::endl;
    std::cout << "TEST CASE: " << testName << std::endl;
    std::cout << "=============================================" << std::endl;

    // 1. 调用你的 AI 模块进行分析
    // 注意：vector.data() 返回指向数组首元素的指针，兼容 int* 接口
    DatasetFeatures features = AIOptimizer::analyzeDataset(data.data(), data.size());
    
    // 2. 获取预测结果
    AlgorithmType prediction = AIOptimizer::predict(features);

    // 3. 打印详细报告
    AIOptimizer::printAnalysisReport(features, prediction);
}

int main() {
    srand(time(0)); // 初始化随机种子

    std::cout << "Starting AI Module Unit Tests...\n" << std::endl;

    // --- 场景 1: 小数据集 (Small Dataset) ---
    // 预期: Insertion Sort (因为 N 小，常数项低)
    std::vector<int> smallData = generateRandom(30);
    runTestCase("Small Random Dataset (N=30)", smallData);

    // --- 场景 2: 几乎有序 (Nearly Sorted) ---
    // 预期: Insertion Sort (利用 O(N) 特性)
    std::vector<int> nearlySortedData = generateNearlySorted(800);
    runTestCase("Nearly Sorted Dataset (N=800)", nearlySortedData);

    // --- 场景 3: 逆序数据 (Reversed) ---
    // 预期: Quick Sort (一定要避开 Insertion Sort 的 O(N^2))
    std::vector<int> reversedData = generateReversed(800);
    runTestCase("Reversed Dataset (N=800)", reversedData);

    // --- 场景 4: 重复元素极多 (Few Unique) ---
    // 预期: Merge Sort (处理重复值更稳定)
    std::vector<int> fewUniqueData = generateFewUnique(2000);
    runTestCase("Few Unique / High Duplication (N=2000)", fewUniqueData);

    // --- 场景 5: 大型随机数据 (Large Random) ---
    // 预期: Quick Sort (平均最快)
    std::vector<int> largeRandomData = generateRandom(5000);
    runTestCase("Large Random Dataset (N=5000)", largeRandomData);

    std::cout << "\nAll tests completed." << std::endl;
    return 0;
}