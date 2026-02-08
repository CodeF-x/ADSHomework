#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cstdint>
#include <set>
#include <iomanip>
#include <fstream>

std::vector<std::vector<double>> memory = std::vector<std::vector<double>>(20, std::vector<double>());

class RandomStreamGen {
private:
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
    std::vector<std::string> a;
public:
    void generate(size_t n) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> s_size(5, 30);
        std::uniform_int_distribution<int> char_index(0, alphabet.size() - 1);
        a.clear();
        for (size_t i = 0; i < n; ++i) {
            std::string s;
            int len = s_size(gen);
            for (int j = 0; j < len; ++j) {
                s += alphabet[char_index(gen)];
            }
            a.push_back(s);
        }
    }

    std::vector<std::string> get_part(double p) const {
        if (p > 1.0) p = 1.0;
        if (p < 0.0) p = 0.0;
        size_t end_idx = static_cast<size_t>(a.size() * p);
        return std::vector<std::string>(a.begin(), a.begin() + end_idx);
    }

    const std::vector<std::string> &get_full() const { return a; }
};


class HashFuncGen {
public:
    static uint32_t hash(const std::string &str) {
        uint32_t basis = 0x811c9dc5;
        uint32_t prime = 0x01000193;
        uint32_t hash = basis;

        for (char c: str) {
            hash ^= static_cast<uint8_t>(c);
            hash *= prime;
        }
        return hash;
    }
};


class HyperLogLog {
private:
    int B;
    int m;
    double alpha;
    std::vector<int> v;

    int count_zeros(uint32_t x, int max_bits) {
        if (x == 0) return max_bits + 1;
        int zeros = 0;
        for (int i = max_bits - 1; i >= 0; --i) {
            if ((x >> i) & 1) break;
            zeros++;
        }
        return zeros + 1;
    }

public:
    HyperLogLog(int b_bits) : B(b_bits) {
        m = 1 << B;
        v = std::vector<int>(m);

        if (B == 4) alpha = 0.673;
        else if (m == 5) alpha = 0.697;
        else if (m == 6) alpha = 0.709;
        else alpha = 0.7213 / (1.0 + 1.079 / m);
    }

    void add(const std::string &str) {
        uint32_t x = HashFuncGen::hash(str);
        uint32_t ind = x >> (32 - B);
        uint32_t ost = x << B;
        int rank = count_zeros(ost, 32 - B);
        v[ind] = std::max(v[ind], rank);
    }

    double estimate() {
        double sum = 0;
        for (int val: v) {
            sum += std::pow(2.0, -val);
        }
        double E = alpha * m * m / sum;
        if (E <= 2.5 * m) {
            int V = 0;
            for (int val: v) if (val == 0) V++;
            if (V > 0) E = m * std::log((double) m / V);
        }
        return E;
    }
};


void experiment() {
    std::ofstream file("result.csv");
    int size = 100000;
    int B = 12;
    RandomStreamGen gen;
    gen.generate(size);
    const auto &stream = gen.get_full();

    HyperLogLog hll(B);
    std::set < std::string > exact;

    file << "step,F0,Nt,error\n";

    double pred = 0;
    for (int p = 5; p <= 100; p += 5) {
        double rat = p / 100.0;
        size_t end = static_cast<size_t>(size * rat);
        size_t start = static_cast<size_t>(size * (p - 5) / 100.0);

        for (size_t i = start; i < end; ++i) {
            hll.add(stream[i]);
            exact.insert(stream[i]);
        }

        pred = hll.estimate();
        size_t f0 = exact.size();
        double error = std::abs(pred - f0) / f0 * 100.0;
        memory[p / 5 - 1].push_back(pred);

        file << p << "," << f0 << "," << std::fixed << std::setprecision(2)
             << pred << "," << error << "\n";
    }
    file.close();
}

int main() {
    int k = 10;
    std::ofstream file("result2.csv");
    file << "step,E,sigma\n";
    for (int i = 0; i < k; ++i) {
        experiment();
    }
    for (int i = 0; i < 20; ++i) {
        double E = 0, sigma = 0;
        for (double j: memory[i]) {
            E += j;
        }
        E /= k;
        for (double j: memory[i]) {
            sigma += (j - E) * (j - E);
        }
        sigma /= k;
        sigma = std::sqrt(sigma);
        file << (i + 1) * 5 << "," << E << "," << std::fixed << std::setprecision(2)
             << sigma << std::fixed << std::setprecision(2) << "\n";
    }

    file.close();
    return 0;
}