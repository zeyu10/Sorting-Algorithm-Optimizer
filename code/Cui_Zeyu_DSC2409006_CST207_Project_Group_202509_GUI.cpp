/*
 * AI-Driven Sorting Algorithm Optimizer - Graphical User Interface
 * qmake SortingAlgorithmOptimizerGUI.pro
 * make
 * ./SortingAlgorithmOptimizerGUI
 */

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

// ============= Data Structure Definitions =============

enum AlgoType {
    BUBBLE_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
};

struct DatasetFeatures {
    vector<int> data;
    int size;
    double sortedness;      // 0.0 (random) to 1.0 (sorted)
    int uniqueCount;        // Number of unique elements
    string type;            // Dataset type
    double reversedness;    // Degree of reverse order (0.0 ~ 1.0)
    double uniqueRatio;     // Ratio of unique elements (0.0 ~ 1.0)
    bool isLargeDataset;    // Large dataset indicator (>1000)
};

struct SortMetrics {
    long long comparisons = 0;      // Number of comparisons
    double executionTimeMs = 0.0;   // Execution time in milliseconds
    string algoName;
};

// ============= Sorting Algorithm Implementations =============

class SortingEngine {
public:
    // Bubble Sort Implementation
    static void bubbleSort(vector<int>& arr, long long& comparisons) {
        int n = arr.size();
        for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                comparisons++;
                if (arr[j] > arr[j + 1]) {
                    swap(arr[j], arr[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) break;  // Early termination if already sorted
        }
    }

    // Insertion Sort Implementation
    static void insertionSort(vector<int>& arr, long long& comparisons) {
        int n = arr.size();
        for (int i = 1; i < n; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= 0) {
                comparisons++;
                if (arr[j] <= key) break;
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    // Merge function for Merge Sort
    static void merge(vector<int>& arr, int l, int m, int r, long long& comparisons) {
        int n1 = m - l + 1;
        int n2 = r - m;
        vector<int> left(n1), right(n2);
        
        for (int i = 0; i < n1; i++) left[i] = arr[l + i];
        for (int j = 0; j < n2; j++) right[j] = arr[m + 1 + j];
        
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            comparisons++;
            if (left[i] <= right[j]) {
                arr[k++] = left[i++];
            } else {
                arr[k++] = right[j++];
            }
        }
        while (i < n1) arr[k++] = left[i++];
        while (j < n2) arr[k++] = right[j++];
    }

    // Merge Sort Implementation
    static void mergeSort(vector<int>& arr, int l, int r, long long& comparisons) {
        if (l >= r) return;
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m, comparisons);
        mergeSort(arr, m + 1, r, comparisons);
        merge(arr, l, m, r, comparisons);
    }

    // Partition function for Quick Sort
    static int partition(vector<int>& arr, int low, int high, long long& comparisons) {
        // Randomize pivot to avoid worst-case on reversed/sorted data
        int randomIndex = low + rand() % (high - low + 1);
        swap(arr[randomIndex], arr[high]);
        
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            comparisons++;
            if (arr[j] < pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    // Quick Sort Implementation
    static void quickSort(vector<int>& arr, int low, int high, long long& comparisons) {
        if (low < high) {
            int pi = partition(arr, low, high, comparisons);
            quickSort(arr, low, pi - 1, comparisons);
            quickSort(arr, pi + 1, high, comparisons);
        }
    }

    // ============= Dataset Generation Functions =============
    
    // Generate random dataset
    static vector<int> generateRandomDataset(int size) {
        vector<int> arr(size);
        srand(time(nullptr));
        for (int i = 0; i < size; i++) {
            arr[i] = 1 + rand() % (size * 10);
        }
        return arr;
    }

    // Generate nearly sorted dataset
    static vector<int> generateNearlySorted(int size) {
        vector<int> arr(size);
        for (int i = 0; i < size; i++) arr[i] = i + 1;
        
        // Disorder about 10% of the elements
        int swaps = size / 10;
        srand(time(nullptr));
        for (int i = 0; i < swaps; i++) {
            int idx1 = rand() % size;
            int idx2 = rand() % size;
            swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }

    // Generate reversed dataset
    static vector<int> generateReversed(int size) {
        vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = size - i;
        }
        return arr;
    }

    // Generate dataset with few unique values
    static vector<int> generateFewUnique(int size, int uniqueCount) {
        vector<int> uniqueValues;
        srand(time(nullptr));
        for (int i = 0; i < uniqueCount; i++) {
            uniqueValues.push_back(rand() % 100 + 1);
        }
        
        vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = uniqueValues[rand() % uniqueCount];
        }
        return arr;
    }

    // ============= AI Analysis Module =============
    
    // Analyze dataset characteristics
    static DatasetFeatures analyzeDataset(const vector<int>& data) {
        DatasetFeatures features;
        features.size = data.size();
        features.data = data;
        features.isLargeDataset = (features.size > 1000);
        
        if (features.size <= 1) {
            features.sortedness = 1.0;
            features.reversedness = 0.0;
            features.uniqueCount = features.size;
            features.uniqueRatio = 1.0;
            features.type = "Single Element";
            return features;
        }
        
        // Calculate sortedness and reversedness
        long long ascendingPairs = 0, descendingPairs = 0;
        for (int i = 0; i < features.size - 1; i++) {
            if (data[i] <= data[i+1]) ascendingPairs++;
            if (data[i] >= data[i+1]) descendingPairs++;
        }
        
        features.sortedness = (double)ascendingPairs / (features.size - 1);
        features.reversedness = (double)descendingPairs / (features.size - 1);
        
        // Calculate uniqueness (use full dataset for accuracy)
        unordered_set<int> uniqueElements(data.begin(), data.end());
        
        features.uniqueCount = uniqueElements.size();
        features.uniqueRatio = (double)uniqueElements.size() / features.size;
        
        // Classify dataset type
        if (features.sortedness >= 0.80) features.type = "Nearly Sorted";
        else if (features.reversedness >= 0.90) features.type = "Reversed";
        else if (features.uniqueRatio < 0.40) features.type = "Few Unique";
        else if (features.isLargeDataset) features.type = "Large Random";
        else features.type = "Random";
        
        return features;
    }

    // Predict best sorting algorithm based on dataset features
    static AlgoType predictBestAlgorithm(const DatasetFeatures& features) {
        // AI Decision Tree based on algorithm complexity theory
        
        // Rule 1: Very small datasets (Size <= 50)
        // Insertion Sort has low constant factor, faster than Quick/Merge recursion overhead
        if (features.size <= 50) {
            return INSERTION_SORT;
        }
        
        // Rule 2: Large datasets (Size > 1000)
        if (features.isLargeDataset) {
            // Few unique values: Merge Sort is more stable than Quick Sort
            if (features.uniqueRatio < 0.40) {
                return MERGE_SORT;
            }
            return QUICK_SORT;
        }
        
        // Rule 3: Medium-sized datasets (50 < Size <= 1000)
        
        // Case A: Nearly sorted
        // Insertion Sort degrades to O(N) for nearly sorted data
        if (features.sortedness >= 0.80) {
            return INSERTION_SORT;
        }
        
        // Case B: Reversed
        // Merge Sort is better for reversed data (stable O(N log N))
        // Quick Sort with fixed pivot has O(N^2) worst case on reversed data
        if (features.reversedness >= 0.90) {
            return MERGE_SORT;
        }
        
        // Case C: Few unique values
        if (features.uniqueRatio < 0.40) {
            return MERGE_SORT;
        }
        
        // Case D: Random data
        return QUICK_SORT;
    }

    // Get algorithm name from type
    static string getAlgoName(AlgoType type) {
        switch (type) {
            case BUBBLE_SORT: return "Bubble Sort";
            case INSERTION_SORT: return "Insertion Sort";
            case MERGE_SORT: return "Merge Sort";
            case QUICK_SORT: return "Quick Sort";
            default: return "Unknown";
        }
    }

    // Run sorting algorithm and measure performance
    static SortMetrics runSort(AlgoType type, vector<int> data) {
        SortMetrics metrics;
        metrics.algoName = getAlgoName(type);
        metrics.comparisons = 0;
        
        auto start = chrono::high_resolution_clock::now();
        
        switch (type) {
            case BUBBLE_SORT: bubbleSort(data, metrics.comparisons); break;
            case INSERTION_SORT: insertionSort(data, metrics.comparisons); break;
            case MERGE_SORT: mergeSort(data, 0, data.size() - 1, metrics.comparisons); break;
            case QUICK_SORT: quickSort(data, 0, data.size() - 1, metrics.comparisons); break;
        }
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        metrics.executionTimeMs = duration.count();
        
        return metrics;
    }
};

// ============= Qt Visualization Interface =============

class SortingVisualizer : public QMainWindow {
    Q_OBJECT

private:
    // UI Components
    QComboBox* datasetTypeCombo;
    QSpinBox* dataSizeSpinBox;
    QSpinBox* uniqueCountSpinBox;
    QLabel* uniqueCountLabel;
    QPushButton* generateBtn;
    QPushButton* runBtn;
    QTextEdit* dataPreviewText;
    QTextEdit* analysisResultText;
    QTableWidget* resultsTable;
    QLabel* statusLabel;
    
    // Data
    vector<int> currentDataset;

public:
    SortingVisualizer(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("AI-Driven Sorting Algorithm Optimizer");
        resize(900, 650);
        
        QWidget* central = new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout* mainLayout = new QVBoxLayout(central);
        
        // Dataset Generation Area
        QGroupBox* genGroup = new QGroupBox("Dataset Generation");
        QHBoxLayout* genLayout = new QHBoxLayout(genGroup);
        
        genLayout->addWidget(new QLabel("Type:"));
        datasetTypeCombo = new QComboBox();
        datasetTypeCombo->addItems({"Random", "Nearly Sorted", "Reversed", "Few Unique", "Large Random"});
        genLayout->addWidget(datasetTypeCombo);
        
        genLayout->addWidget(new QLabel("Size:"));
        dataSizeSpinBox = new QSpinBox();
        dataSizeSpinBox->setRange(10, 100000);
        dataSizeSpinBox->setValue(1000);
        dataSizeSpinBox->setSingleStep(100);
        genLayout->addWidget(dataSizeSpinBox);
        
        uniqueCountLabel = new QLabel("Unique:");
        uniqueCountSpinBox = new QSpinBox();
        uniqueCountSpinBox->setRange(2, 50);
        uniqueCountSpinBox->setValue(5);
        uniqueCountSpinBox->setEnabled(false);
        genLayout->addWidget(uniqueCountLabel);
        genLayout->addWidget(uniqueCountSpinBox);
        
        generateBtn = new QPushButton("Generate Dataset");
        generateBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
        genLayout->addWidget(generateBtn);
        
        runBtn = new QPushButton("Run Analysis and Sort");
        runBtn->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold; padding: 8px;");
        runBtn->setEnabled(false);
        genLayout->addWidget(runBtn);
        
        genLayout->addStretch();
        mainLayout->addWidget(genGroup);
        
        // Data Preview Section
        QGroupBox* previewGroup = new QGroupBox("Data Preview");
        QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);
        dataPreviewText = new QTextEdit();
        dataPreviewText->setReadOnly(true);
        dataPreviewText->setMinimumHeight(100);
        dataPreviewText->setPlaceholderText("Click 'Generate Dataset' to start...");
        previewLayout->addWidget(dataPreviewText);
        mainLayout->addWidget(previewGroup);
        
        // AI Analysis Results Section
        QGroupBox* analysisGroup = new QGroupBox("AI Analysis Results");
        QVBoxLayout* analysisLayout = new QVBoxLayout(analysisGroup);
        analysisResultText = new QTextEdit();
        analysisResultText->setReadOnly(true);
        analysisResultText->setMinimumHeight(120);
        analysisLayout->addWidget(analysisResultText);
        mainLayout->addWidget(analysisGroup);
        
        // Performance Comparison Table
        QGroupBox* resultsGroup = new QGroupBox("Sorting Performance Comparison");
        QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);
        resultsTable = new QTableWidget();
        resultsTable->setColumnCount(3);
        resultsTable->setHorizontalHeaderLabels({"Algorithm", "Comparisons", "Time(ms)"});
        resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultsTable->setMinimumHeight(240);
        resultsLayout->addWidget(resultsTable);
        mainLayout->addWidget(resultsGroup);
        
        // Status Bar
        statusLabel = new QLabel("Ready");
        mainLayout->addWidget(statusLabel);
        
        // Connect Signals and Slots
        connect(generateBtn, &QPushButton::clicked, this, &SortingVisualizer::onGenerate);
        connect(runBtn, &QPushButton::clicked, this, &SortingVisualizer::onRun);
        connect(datasetTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
            // Enable unique count spinbox only for "Few Unique" (index 3)
            uniqueCountSpinBox->setEnabled(index == 3);
            uniqueCountLabel->setEnabled(index == 3);
        });
    }

private slots:
    void onGenerate() {
        int size = dataSizeSpinBox->value();
        int type = datasetTypeCombo->currentIndex();
        
        statusLabel->setText("Generating...");
        
        try {
            // Generate dataset based on selected type
            switch(type) {
                case 0: currentDataset = SortingEngine::generateRandomDataset(size); break;
                case 1: currentDataset = SortingEngine::generateNearlySorted(size); break;
                case 2: currentDataset = SortingEngine::generateReversed(size); break;
                case 3: currentDataset = SortingEngine::generateFewUnique(size, uniqueCountSpinBox->value()); break;
                case 4: 
                    // Large Random Dataset
                    if (size < 10000) {
                        QMessageBox::information(this, "Info", "Large Random Dataset requires minimum size of 10000. Size adjusted to 10000.");
                        size = 10000;
                        dataSizeSpinBox->setValue(10000);
                    }
                    currentDataset = SortingEngine::generateRandomDataset(size);
                    break;
            }
            
            // Display Preview
            ostringstream oss;
            int preview = min(40, size);
            oss << "Size: " << size << " | First " << preview << " elements: [";
            for (int i = 0; i < preview; i++) {
                oss << currentDataset[i];
                if (i < preview - 1) oss << ", ";
            }
            if (size > preview) oss << ", ...";
            oss << "]";
            dataPreviewText->setText(QString::fromStdString(oss.str()));
            
            runBtn->setEnabled(true);
            statusLabel->setText("Dataset Generated Successfully");
            analysisResultText->clear();
            resultsTable->setRowCount(0);
            
        } catch (const exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            statusLabel->setText("Generation Failed");
        }
    }

    void onRun() {
        if (currentDataset.empty()) {
            QMessageBox::warning(this, "Warning", "Please generate a dataset first");
            return;
        }
        
        statusLabel->setText("Analyzing...");
        QApplication::processEvents();
        
        // AI Analysis
        DatasetFeatures features = SortingEngine::analyzeDataset(currentDataset);
        AlgoType predicted = SortingEngine::predictBestAlgorithm(features);
        
        // Display analysis results
        ostringstream oss;
        oss << "[Dataset Features]\n";
        oss << "Type: " << features.type << " | ";
        oss << "Size: " << features.size << (features.isLargeDataset ? " (Large)" : " (Small/Medium)") << "\n";
        oss << "Sortedness: " << fixed << setprecision(1) << (features.sortedness * 100) << "% | ";
        oss << "Reversedness: " << (features.reversedness * 100) << "% | ";
        oss << "Uniqueness: " << (features.uniqueRatio * 100) << "%\n\n";
        oss << "[AI Prediction] Optimal Algorithm: " << SortingEngine::getAlgoName(predicted);
        
        analysisResultText->setText(QString::fromStdString(oss.str()));
        
        statusLabel->setText("Sorting...");
        QApplication::processEvents();
        
        // Run Sorting Algorithms
        vector<SortMetrics> results;
        int size = currentDataset.size();
        
        // Skip O(n^2) algorithms for large datasets
        if (size <= 1000) {
            results.push_back(SortingEngine::runSort(BUBBLE_SORT, currentDataset));
            results.push_back(SortingEngine::runSort(INSERTION_SORT, currentDataset));
        }
        results.push_back(SortingEngine::runSort(MERGE_SORT, currentDataset));
        results.push_back(SortingEngine::runSort(QUICK_SORT, currentDataset));
        
        // Find the best performing algorithm
        string actualBest;
        double minTime = 1e9;
        for (const auto& r : results) {
            if (r.executionTimeMs < minTime) {
                minTime = r.executionTimeMs;
                actualBest = r.algoName;
            }
        }
        
        // Display Results in Table
        resultsTable->setRowCount(results.size());
        for (size_t i = 0; i < results.size(); i++) {
            QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(results[i].algoName));
            QTableWidgetItem* compItem = new QTableWidgetItem(QString::number(results[i].comparisons));
            QTableWidgetItem* timeItem = new QTableWidgetItem(QString::number(results[i].executionTimeMs, 'f', 4));
            
            // Highlight the best performing algorithm
            if (results[i].algoName == actualBest) {
                QBrush gold(QColor(255, 215, 0, 120));
                nameItem->setBackground(gold);
                compItem->setBackground(gold);
                timeItem->setBackground(gold);
            }
            
            resultsTable->setItem(i, 0, nameItem);
            resultsTable->setItem(i, 1, compItem);
            resultsTable->setItem(i, 2, timeItem);
        }
        
        // Update status with prediction accuracy
        string predictedName = SortingEngine::getAlgoName(predicted);
        if (predictedName == actualBest) {
            statusLabel->setText("Complete | AI Prediction Correct! Best: " + QString::fromStdString(actualBest));
        } else {
            statusLabel->setText("Complete | Predicted: " + QString::fromStdString(predictedName) + 
                               " -> Actual Best: " + QString::fromStdString(actualBest));
        }
    }
};

// ============= Main Function =============

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SortingVisualizer window;
    window.show();
    return app.exec();
}

#include "Cui_Zeyu_DSC2409006_CST207_Project_Group_202509_GUI.moc"
