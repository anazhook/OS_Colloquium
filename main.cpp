#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <windows.h>
#include <mutex>

using namespace std;

const string OUTPUT_FILE = "out.dat";
mutex resultMutex;

// Функция выполнения операций в отдельном потоке
void processFile(const string& filename, double& result) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    int action;
    file >> action;

    double value, partialResult = 0;

    while (file >> value) {
        switch (action) {
        case 1: // сложение
            partialResult += value;
            break;
        case 2: // умножение
            partialResult *= value;
            break;
        case 3: // сумма квадратов
            partialResult += value * value;
            break;
        default:
            cerr << "Invalid action code: " << action << endl;
            break;
        }
    }

    file.close();

    // Захватываем мьютекс перед обновлением общего результата
    lock_guard<mutex> lock(resultMutex);
    result += partialResult;

    cout << "Processed file: " << filename << ", Partial Result: " << partialResult << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <directory_path> <num_threads>" << endl;
        return 1;
    }

    string directoryPath = argv[1];
    int numThreads = atoi(argv[2]);

    if (numThreads <= 0) {
        cerr << "Invalid number of threads." << endl;
        return 1;
    }

    vector<thread> threads;
    vector<double> threadResults(numThreads, 0.0);

    wstring searchPattern = wstring(directoryPath.begin(), directoryPath.end()) + L"\\in_*.dat";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPattern.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        cerr << "Error searching for input files in the directory: " << directoryPath << endl;
        return 1;
    }

    do {
        wstring filenameW = wstring(directoryPath.begin(), directoryPath.end()) + L"\\" + findFileData.cFileName;
        string filename(filenameW.begin(), filenameW.end());

        int threadIndex = findFileData.cFileName[3] - '0';

        threads.emplace_back(processFile, filename, ref(threadResults[threadIndex]));

    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Вычисление общей суммы результатов
    double totalResult = 0.0;
    for (double result : threadResults) {
        totalResult += result;
    }

    // Запись результата в файл out.dat
    ofstream out("out.dat");
    out << "jyhtjhtdfdfghjk";
    cout << totalResult;
    if (out.is_open()) {
        cout << totalResult << endl;
        out.close();
        cout << "Calculation completed successfully. Result written to " << OUTPUT_FILE << endl;
    }
    else {
        cerr << "Error opening output file: " << OUTPUT_FILE << endl;
        return 1;
    }

    return 0;
}
