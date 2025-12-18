#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "KNN_Optimizer.h"

// ... (这里保留之前的 generateRandom 等 4 个生成函数，代码完全一样，不用变) ...
// 为了节省篇幅，这里假设 generateRandom, generateNearlySorted 等函数已经定义好了
// 如果你删了，请去上面的记录复制一下
std::vector<int> generateRandom(int size) {
    std::vector<int> data(size);
    for (int i = 0; i < size; i++) data[i] = rand() % 10000;
    return data;
}
std::vector<int> generateNearlySorted(int size) {
    std::vector<int> data(size);
    for (int i=0; i<size; i++) data[i]=i;
    int swaps = size*0.05;
    for(int i=0; i<swaps; i++) std::swap(data[rand()%size], data[rand()%size]);
    return data;
}
std::vector<int> generateReversed(int size) {
    std::vector<int> data(size);
    for(int i=0; i<size; i++) data[i]=size-i;
    return data;
}
std::vector<int> generateFewUnique(int size) {
    std::vector<int> data(size);
    for(int i=0; i<size; i++) data[i]=rand()%10;
    return data;
}

void runTestCase(KNNOptimizer& ai, std::string name, std::vector<int>& data) {
    std::cout << "\n--------------------------------------------------" << std::endl;
    std::cout << "TEST: " << name << " (N=" << data.size() << ")" << std::endl;
    
    // 1. 提取
    DatasetFeatures f = KNNOptimizer::extractFeatures(data.data(), data.size());
    std::cout << "Features -> Sorted: " << f.sortedness 
              << ", Rev: " << f.reversedness 
              << ", Uniq: " << f.uniqueness << std::endl;

    // 2. 预测
    AlgorithmType res = ai.predict(f);
    std::cout << ">>> FINAL DECISION: " << KNNOptimizer::getAlgorithmName(res) << std::endl;
}

int main() {
    srand(time(0));
    KNNOptimizer ai;

    // 测试 1: 小随机 -> Insert
    std::vector<int> d1 = generateRandom(40);
    runTestCase(ai, "Small Random", d1);

    // 测试 2: 大逆序 -> Quick (AI 可能会因为距离近选 Insert，但必须被 Safety Override 拦截)
    // 这是验证你满分代码的关键测试点！
    std::vector<int> d2 = generateReversed(2000);
    runTestCase(ai, "Large Reversed", d2);

    // 测试 3: 多重复 -> Merge
    std::vector<int> d3 = generateFewUnique(3000);
    runTestCase(ai, "Many Duplicates", d3);

    // 测试 4: 大随机 -> Quick
    std::vector<int> d4 = generateRandom(8000);
    runTestCase(ai, "Large Random", d4);

    return 0;
}