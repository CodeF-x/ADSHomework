#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>

using namespace std;

class StringGenerator {
private:
    string alphabet;
    mt19937 rng;

public:
    StringGenerator() : rng(1337) {
        for (char c = 'A'; c <= 'Z'; ++c) alphabet += c;
        for (char c = 'a'; c <= 'z'; ++c) alphabet += c;
        for (char c = '0'; c <= '9'; ++c) alphabet += c;
        string spec = "!@#%:;^&*()-.";
        alphabet += spec;
    }

    string generateString(size_t min_len, size_t max_len) {
        uniform_int_distribution<size_t> len_dist(min_len, max_len);
        uniform_int_distribution<size_t> char_dist(0, alphabet.size() - 1);
        size_t len = len_dist(rng);
        string s = "";
        for (size_t i = 0; i < len; ++i) {
            s += alphabet[char_dist(rng)];
        }
        return s;
    }

    vector<string> generateBase(const string &type, size_t min_len = 10, size_t max_len = 200) {
        vector<string> data(3000);
        for (size_t i = 0; i < 3000; ++i) {
            data[i] = generateString(min_len, max_len);
        }

        if (type == "random") {
        } else if (type == "reversed") {
            sort(data.begin(), data.end(), greater<string>());
        } else if (type == "nearly_sorted") {
            sort(data.begin(), data.end());
            uniform_int_distribution<size_t> idx_dist(0, 2999);
            for (int i = 0; i < 150; ++i) {
                swap(data[idx_dist(rng)], data[idx_dist(rng)]);
            }
        } else if (type == "prefix") {
            string prefix = generateString(30, 30);
            for (size_t i = 0; i < 3000; ++i) {
                data[i] = prefix + generateString(min_len, max_len - 30);
            }
        }
        return data;
    }
};

class StringSortTester {
public:
    static long long char_comps;

    static int AtRadix(const string &s, int depth) {
        if (depth < s.length()) return (unsigned char) s[depth] + 1;
        return 0;
    }

    static bool standardCompare(const string &a, const string &b) {
        size_t min_l = min(a.length(), b.length());
        for (size_t i = 0; i < min_l; ++i) {
            char_comps++;
            if (a[i] != b[i]) return a[i] < b[i];
        }
        char_comps++;
        return a.length() < b.length();
    }

    static void standardQuickSort(vector<string> &arr, int low, int high) {
        if (low >= high) return;
        string pt = arr[low + (high - low) / 2];
        int i = low, j = high;
        while (i <= j) {
            while (standardCompare(arr[i], pt)) i++;
            while (standardCompare(pt, arr[j])) j--;
            if (i <= j) {
                swap(arr[i], arr[j]);
                i++;
                j--;
            }
        }
        standardQuickSort(arr, low, j);
        standardQuickSort(arr, i, high);
    }

    static void standardMerge(vector<string> &arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        vector<string> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (standardCompare(L[i], R[j])) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    static void standardMergeSort(vector<string> &arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        standardMergeSort(arr, left, mid);
        standardMergeSort(arr, mid + 1, right);
        standardMerge(arr, left, mid, right);
    }

    static void stringQuickSort(vector<string> &arr, int low, int high, int depth) {
        if (high <= low) return;
        int lt = low;
        int gt = high;
        int pt = AtRadix(arr[low], depth);
        int i = low + 1;
        while (i <= gt) {
            int t = AtRadix(arr[i], depth);
            char_comps++;
            if (t < pt) {
                swap(arr[lt++], arr[i++]);
            } else if (t > pt) {
                swap(arr[i], arr[gt--]);
            } else {
                i++;
            }
        }
        stringQuickSort(arr, low, lt - 1, depth);
        if (pt > 0) {
            stringQuickSort(arr, lt, gt, depth + 1);
        }
        stringQuickSort(arr, gt + 1, high, depth);
    }

    static int lcpCompare(const string &a, const string &b, int start_depth) {
        int depth = start_depth;
        int min_len = min(a.length(), b.length());
        while (depth < min_len) {
            char_comps++;
            if (a[depth] != b[depth]) {
                return depth;
            }
            depth++;
        }
        char_comps++;
        return depth;
    }

    static void stringMerge(vector<string> &arr, int left, int mid, int right,
                            vector<int> &lcp, vector<string> &extra, vector<int> &extra_lcp) {
        int i = left;
        int j = mid + 1;
        int k = left;
        int lcp_ij = lcpCompare(arr[i], arr[j], 0);
        while (i <= mid && j <= right) {
            if (lcp_ij < arr[i].length() && (lcp_ij == arr[j].length() || arr[i][lcp_ij] < arr[j][lcp_ij])) {
                extra[k] = arr[i];
                if (i > left) {
                    extra_lcp[k - 1] = lcp[i - 1];
                }
                if (i < mid) {
                    int lcp_next = lcp[i];
                    if (lcp_next > lcp_ij) {
                    } else if (lcp_next < lcp_ij) {
                        lcp_ij = lcp_next;
                    } else {
                        lcp_ij = lcpCompare(arr[i + 1], arr[j], lcp_ij);
                    }
                }
                i++;
            } else {
                extra[k] = arr[j];
                if (k > left) {
                    extra_lcp[k - 1] = lcp_ij;
                }
                if (j < right) {
                    int lcp_next = lcp[j];
                    if (lcp_next > lcp_ij) {
                    } else if (lcp_next < lcp_ij) {
                        lcp_ij = lcp_next;
                    } else {
                        lcp_ij = lcpCompare(arr[i], arr[j + 1], lcp_ij);
                    }
                }
                j++;
            }
            k++;
        }
        while (i <= mid) {
            extra[k] = arr[i];
            if (k > left) extra_lcp[k - 1] = lcp[i - 1];
            i++;
            k++;
        }
        while (j <= right) {
            extra[k] = arr[j];
            if (k > left) extra_lcp[k - 1] = lcp[j - 1];
            j++;
            k++;
        }
        for (int m = left; m <= right; m++) {
            arr[m] = extra[m];
        }
        for (int m = left; m < right; m++) {
            lcp[m] = lcpCompare(arr[m], arr[m + 1], 0);
        }
    }

    static void stringMergeSortSplit(vector<string> &arr, int left, int right,
                                     vector<int> &lcp, vector<string> &extra, vector<int> &extra_lcp) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        stringMergeSortSplit(arr, left, mid, lcp, extra, extra_lcp);
        stringMergeSortSplit(arr, mid + 1, right, lcp, extra, extra_lcp);
        stringMerge(arr, left, mid, right, lcp, extra, extra_lcp);
    }

    static void stringMergeSortLCP(vector<string> &arr) {
        int n = arr.size();
        if (n <= 1) return;
        vector<int> lcp(n - 1, 0);
        vector<string> extra(n);
        vector<int> extra_lcp(n - 1, 0);
        for (int i = 0; i < n - 1; i++) {
            lcp[i] = lcpCompare(arr[i], arr[i + 1], 0);
        }
        stringMergeSortSplit(arr, 0, n - 1, lcp, extra, extra_lcp);
    }

    static void msdRadixSort(vector<string> &arr, int low, int high, int depth, vector<string> &aux, int R = 257) {
        if (high <= low) return;
        vector<int> count(R + 2, 0);
        for (int i = low; i <= high; i++) {
            count[AtRadix(arr[i], depth) + 1]++;
            char_comps++;
        }
        for (int r = 0; r < R + 1; r++) count[r + 1] += count[r];
        for (int i = low; i <= high; i++) aux[low + count[AtRadix(arr[i], depth)]++] = arr[i];
        for (int i = low; i <= high; i++) arr[i] = aux[i];
        for (int r = 1; r < R; r++) {
            msdRadixSort(arr, low + count[r - 1], low + count[r] - 1, depth + 1, aux, R);
        }
    }

    static void
    msdRadixSortHybrid(vector<string> &arr, int low, int high, int depth, vector<string> &aux, int R = 257) {
        if (high <= low) return;
        if (high - low + 1 < 74) {
            stringQuickSort(arr, low, high, depth);
            return;
        }
        vector<int> count(R + 2, 0);
        for (int i = low; i <= high; i++) {
            count[AtRadix(arr[i], depth) + 1]++;
            char_comps++;
        }
        for (int r = 0; r < R + 1; r++) count[r + 1] += count[r];
        for (int i = low; i <= high; i++) aux[low + count[AtRadix(arr[i], depth)]++] = arr[i];
        for (int i = low; i <= high; i++) arr[i] = aux[i];
        for (int r = 1; r < R; r++) {
            msdRadixSortHybrid(arr, low + count[r - 1], low + count[r] - 1, depth + 1, aux, R);
        }
    }
};

long long StringSortTester::char_comps = 0;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    StringGenerator generator;

    string data[] = {"random", "reversed", "nearly_sorted", "prefix"};
    string algorithms[] = {"Std_QS", "Std_MS", "String_QS", "String_MS_LCP", "MSD_Radix", "MSD_Hybrid"};
    int repets = 10;
    for (const string &algo: algorithms) {
        string filename = algo + "_results.csv";
        ofstream file(filename);

        file << "Data,Size,ms,comparisons\n";
        for (const string &type: data) {
            auto master_array = generator.generateBase(type);
            for (int size = 100; size <= 3000; size += 100) {
                vector<string> sub_arr(master_array.begin(), master_array.begin() + size);
                double total_time_ms = 0.0;
                long long total_comparisons = 0;
                for (int r = 0; r < repets; ++r) {
                    vector<string> test_copy = sub_arr;
                    vector<string> aux(size);
                    StringSortTester::char_comps = 0;
                    auto start = chrono::high_resolution_clock::now();
                    if (algo == "Std_QS") {
                        StringSortTester::standardQuickSort(test_copy, 0, size - 1);
                    } else if (algo == "Std_MS") {
                        StringSortTester::standardMergeSort(test_copy, 0, size - 1);
                    } else if (algo == "String_QS") {
                        StringSortTester::stringQuickSort(test_copy, 0, size - 1, 0);
                    } else if (algo == "String_MS_LCP") {
                        StringSortTester::stringMergeSortLCP(test_copy);
                    } else if (algo == "MSD_Radix") {
                        StringSortTester::msdRadixSort(test_copy, 0, size - 1, 0, aux);
                    } else if (algo == "MSD_Hybrid") {
                        StringSortTester::msdRadixSortHybrid(test_copy, 0, size - 1, 0, aux);
                    }
                    auto end = chrono::high_resolution_clock::now();
                    double time_taken = chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;
                    total_time_ms += time_taken;
                    total_comparisons += StringSortTester::char_comps;
                }
                double time = total_time_ms / repets;
                long long comps = total_comparisons / repets;
                file << type << ","
                     << size << ","
                     << fixed << setprecision(4) << time << ","
                     << comps << "\n";
            }
        }
        file.close();
    }
    return 0;
}