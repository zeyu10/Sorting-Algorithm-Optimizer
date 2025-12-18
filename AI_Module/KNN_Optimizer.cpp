#include "KNN_Optimizer.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_set>

// 构造函数：在这里“硬编码”训练数据
// 我们根据算法理论，生成一些典型的点放入知识库
KNNOptimizer::KNNOptimizer() {
    // 格式: {Size(0-1), Sortedness, Reversed, Unique, Best_Algo}
    // 假设最大 Size = 10000，所以 50 => 0.005, 5000 => 0.5
    
    // --- 类别 1: 小数据集 (Size < 100) ---
    // 理论：Insertion Sort 最快
    trainingData.push_back({0.001, 0.1, 0.1, 1.0, INSERTION_SORT}); // 极小，乱序
    trainingData.push_back({0.005, 0.9, 0.0, 1.0, INSERTION_SORT}); // 极小，有序
    trainingData.push_back({0.005, 0.0, 0.9, 1.0, INSERTION_SORT}); // 极小，逆序
    trainingData.push_back({0.010, 0.5, 0.5, 1.0, INSERTION_SORT}); // 小，随机

    // --- 类别 2: 几乎有序 (Nearly Sorted) ---
    // 理论：Insertion Sort (O(N))
    trainingData.push_back({0.5, 0.95, 0.0, 1.0, INSERTION_SORT}); // 中等大小，非常有序
    trainingData.push_back({0.8, 0.92, 0.0, 1.0, INSERTION_SORT}); // 大数据，有序
    trainingData.push_back({0.2, 0.99, 0.0, 1.0, INSERTION_SORT}); 

    // --- 类别 3: 逆序 (Reversed) ---
    // 理论：Quick Sort 或 Merge (绝对避开 Insertion)
    trainingData.push_back({0.5, 0.0, 0.95, 1.0, QUICK_SORT}); 
    trainingData.push_back({0.8, 0.0, 0.99, 1.0, QUICK_SORT});
    trainingData.push_back({0.2, 0.0, 0.90, 1.0, QUICK_SORT});

    // --- 类别 4: 重复元素多 (Few Unique) ---
    // 理论：Merge Sort (更稳定)
    trainingData.push_back({0.5, 0.4, 0.4, 0.1, MERGE_SORT}); // 只有 10% 唯一
    trainingData.push_back({0.9, 0.5, 0.5, 0.05, MERGE_SORT}); // 只有 5% 唯一
    trainingData.push_back({0.3, 0.2, 0.2, 0.2, MERGE_SORT}); 

    // --- 类别 5: 大规模随机数据 (Large Random) ---
    // 理论：Quick Sort
    trainingData.push_back({0.5, 0.5, 0.1, 1.0, QUICK_SORT}); 
    trainingData.push_back({0.8, 0.4, 0.2, 0.9, QUICK_SORT}); 
    trainingData.push_back({0.9, 0.6, 0.1, 1.0, QUICK_SORT}); 
    trainingData.push_back({0.3, 0.1, 0.1, 1.0, QUICK_SORT}); 
}

// 辅助结构：用于存储距离和对应的标签，方便排序
struct Neighbor {
    double distance;
    AlgorithmType algo;
};

// 比较函数：按距离从小到大排序
bool compareNeighbors(const Neighbor& a, const Neighbor& b) {
    return a.distance < b.distance;
}

// 核心预测函数
AlgorithmType KNNOptimizer::predict(DatasetFeatures input, int k) {
    std::vector<Neighbor> distances;

    // 1. 计算输入点与所有训练点的距离
    for (const auto& sample : trainingData) {
        double dist = calculateDistance(sample, input);
        distances.push_back({dist, sample.bestAlgo});
    }

    // 2. 排序，找到最近的 k 个
    std::sort(distances.begin(), distances.end(), compareNeighbors);

    // 3. 投票 (Majority Voting)
    int votes[4] = {0, 0, 0, 0}; // 对应 4 种算法的票数
    
    // 防止 k 超过样本总数
    if (k > distances.size()) k = distances.size();

    std::cout << "\n[k-NN Debug] Nearest " << k << " neighbors found:" << std::endl;
    for (int i = 0; i < k; i++) {
        votes[distances[i].algo]++;
        std::cout << "  Neighbor " << i+1 << ": Distance=" << distances[i].distance 
                  << " | Vote=" << getAlgorithmName(distances[i].algo) << std::endl;
    }

    // 4. 找出票数最多的算法
    int maxVotes = -1;
    AlgorithmType bestAlgo = QUICK_SORT; // 默认

    for (int i = 0; i < 4; i++) {
        if (votes[i] > maxVotes) {
            maxVotes = votes[i];
            bestAlgo = (AlgorithmType)i;
        }
    }

    return bestAlgo;
}

// 欧几里得距离计算
double KNNOptimizer::calculateDistance(const TrainingSample& sample, const DatasetFeatures& input) {
    // 注意：Input 的 Size 需要在这里归一化，保持和 Training Data 一致
    // 假设最大 Size 是 10000 (这只是一个缩放因子，只要统一就行)
    double inputNormSize = (double)input.originalSize / 10000.0;
    if (inputNormSize > 1.0) inputNormSize = 1.0; // 封顶

    double dSize = sample.normSize - inputNormSize;
    double dSort = sample.sortedness - input.sortedness;
    double dRev  = sample.reversedness - input.reversedness;
    double dUniq = sample.uniqueness - input.uniqueness;

    // 标准欧几里得公式: sqrt(x^2 + y^2 + ...)
    // 我们可以给某些特征加权重，比如 Sortedness 很重要，可以乘 2
    return std::sqrt(dSize*dSize + dSort*dSort*2.0 + dRev*dRev*2.0 + dUniq*dUniq);
}

// 特征提取 (和之前的逻辑一样)
DatasetFeatures KNNOptimizer::extractFeatures(int* arr, int n) {
    DatasetFeatures f;
    f.originalSize = n;
    
    if (n <= 1) {
        f.sortedness = 1.0; f.reversedness = 0.0; f.uniqueness = 1.0;
        return f;
    }

    long long asc = 0, desc = 0;
    for (int i=0; i<n-1; i++) {
        if (arr[i] <= arr[i+1]) asc++;
        if (arr[i] >= arr[i+1]) desc++;
    }
    f.sortedness = (double)asc / (n-1);
    f.reversedness = (double)desc / (n-1);

    int sample = (n < 100) ? n : 100;
    std::unordered_set<int> u;
    for(int i=0; i<sample; i++) u.insert(arr[i]);
    f.uniqueness = (double)u.size() / sample;

    return f;
}

std::string KNNOptimizer::getAlgorithmName(AlgorithmType type) {
    switch (type) {
        case BUBBLE_SORT: return "Bubble Sort";
        case INSERTION_SORT: return "Insertion Sort";
        case MERGE_SORT: return "Merge Sort";
        case QUICK_SORT: return "Quick Sort";
        default: return "Unknown";
    }
}