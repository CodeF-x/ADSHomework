#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <iomanip>
#include <fstream>
#include <algorithm>

void merge(std::vector<int> &a, int l, int m, int r) {
    std::vector<int> left(a.begin() + l, a.begin() + m + 1);
    std::vector<int> right(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            a[k] = left[i];
            i++;
            k++;
        } else {
            a[k] = right[j];
            k++;
            j++;
        }
    }
    while (i < left.size()) {
        a[k] = left[i];
        i++;
        k++;
    }
    while (j < right.size()) {
        a[k] = right[j];
        k++;
        j++;
    }
}

void M_Sort(std::vector<int> &arr, int l, int r) {
    if (l >= r) {
        return;
    }
    int m = l + (r - l) / 2;
    M_Sort(arr, l, m);
    M_Sort(arr, m + 1, r);
    merge(arr, l, m, r);
}

void I_Sort(std::vector<int> &a, int l, int r) {
    for (int i = l + 1; i <= r; ++i) {
        int temp = a[i];
        int j = i - 1;
        while (j >= l && a[j] > temp) {
            a[j + 1] = a[j];
            j -= 1;
        };
        a[j + 1] = temp;
    }
}

void H_M_Sort(std::vector<int> &a, int l, int r, int t = 20) {
    if (r - l + 1 <= t) {
        I_Sort(a, l, r);
        return;
    }
    int m = l + (r - l) / 2;
    H_M_Sort(a, l, m, t);
    H_M_Sort(a, m + 1, r, t);
    merge(a, l, m, r);
}

class ArrayGenerator {
    std::mt19937 gen;
public:
    ArrayGenerator() : gen(std::random_device{}()) {}

    std::vector<int> randomArray(int n) {
        std::uniform_int_distribution<int> dist(0, 6000);
        std::vector<int> a(n);
        for (int i = 0; i < n; ++i) {
            a[i] = dist(gen);
        }
        return a;
    }

    std::vector<int> reverseSortedArray(int n) {
        std::vector<int> a = randomArray(n);
        std::sort(a.begin(), a.end());
        std::reverse(a.begin(), a.end());
        return a;
    }


    std::vector<int> almostSortedArray(int n) {
        std::vector<int> a = randomArray(n);
        std::uniform_int_distribution<int> dist(0, n - 1);
        for (int i = 0; i < 20; ++i) {
            int f = dist(gen);
            int s = dist(gen);
            std::swap(a[f], a[s]);
        }
        return a;
    }
};


int main() {
    std::ofstream file1("SET3_A2_randArr.csv");
    file1 << "N,time\n";
    std::ofstream file2("SET3_A2_rev_randArr.csv");
    file2 << "N,time\n";
    std::ofstream file3("SET3_A2_almostSortedArr.csv");
    file3 << "N,time\n";
    std::random_device seed;
    std::mt19937 gen(seed());
    ArrayGenerator arrG = ArrayGenerator();
    std::vector<int> a = arrG.randomArray(100000);
    std::vector<int> b = arrG.reverseSortedArray(100000);
    std::vector<int> c = arrG.almostSortedArray(100000);
    for (int i = 500; i < 100000; i += 100) {
        std::uniform_int_distribution<> dist(0, 100000 - i - 1);
        int l = dist(gen);
        auto start = std::chrono::high_resolution_clock::now();
        M_Sort(a, l, l + i);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        file1 << i << "," << msec << "\n";
    }
    for (int i = 500; i < 100000; i += 100) {
        std::uniform_int_distribution<> dist(0, 100000 - i - 1);
        int l = dist(gen);
        auto start = std::chrono::high_resolution_clock::now();
        M_Sort(b, l, l + i);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        file2 << i << "," << msec << "\n";
    }
    for (int i = 500; i < 100000; i += 100) {
        std::uniform_int_distribution<> dist(0, 100000 - i - 1);
        int l = dist(gen);
        auto start = std::chrono::high_resolution_clock::now();
        M_Sort(c, l, l + i);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        file3 << i << "," << msec << "\n";
    }


    file1.close();
    file2.close();
    file3.close();
    return 0;
}