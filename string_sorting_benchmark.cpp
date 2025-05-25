#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double, std::micro>;
static uint64_t char_comparisons = 0;
const int TRIALS = 7;
const int MSD_CUTOFF = 64;
const int R = 256;

class StringGenerator {
public:
    StringGenerator()
            : eng_(std::random_device{}()),
              len_dist_(10, 200),
              char_dist_(0, chars_.size() - 1) {}

    std::vector<std::string> generateRandom(size_t N) {
        std::vector<std::string> v; v.reserve(N);
        for (size_t i = 0; i < N; ++i) v.push_back(randomString());
        return v;
    }
    std::vector<std::string> generateReversed(const std::vector<std::string>& src) {
        auto v = src; std::reverse(v.begin(), v.end()); return v;
    }
    std::vector<std::string> generateAlmostSorted(const std::vector<std::string>& src, size_t swaps) {
        auto v = src;
        std::uniform_int_distribution<size_t> d(0, v.size() - 1);
        for (size_t i = 0; i < swaps; ++i)
            std::swap(v[d(eng_)], v[d(eng_)]);
        return v;
    }

private:
    const std::string chars_ =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "!@#%:;^&*()-";
    std::mt19937 eng_;
    std::uniform_int_distribution<size_t> len_dist_, char_dist_;

    std::string randomString() {
        size_t len = len_dist_(eng_);
        std::string s; s.reserve(len);
        for (size_t i = 0; i < len; ++i)
            s.push_back(chars_[char_dist_(eng_)]);
        return s;
    }
};

struct CountingComparator {
    bool operator()(const std::string& a, const std::string& b) const {
        size_t n = std::min(a.size(), b.size());
        for (size_t i = 0; i < n; ++i) {
            ++char_comparisons;
            if (a[i] != b[i]) return a[i] < b[i];
        }
        if (a.size() != b.size()) ++char_comparisons;
        return a.size() < b.size();
    }
};

const std::string& choosePivot(const std::vector<std::string>& a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    // ensure a[lo] <= a[mid] <= a[hi]
    if (a[mid] < a[lo]) std::swap(const_cast<std::string&>(a[mid]), const_cast<std::string&>(a[lo]));
    if (a[hi] < a[lo]) std::swap(const_cast<std::string&>(a[hi]), const_cast<std::string&>(a[lo]));
    if (a[hi] < a[mid]) std::swap(const_cast<std::string&>(a[hi]), const_cast<std::string&>(a[mid]));
    return a[mid];
}

void quickSortStd(std::vector<std::string>& a, int lo, int hi) {
    if (lo >= hi) return;
    const std::string& pivot = choosePivot(a, lo, hi);
    int i = lo, j = hi;
    while (i <= j) {
        while (CountingComparator()(a[i], pivot)) ++i;
        while (CountingComparator()(pivot, a[j])) --j;
        if (i <= j) std::swap(a[i++], a[j--]);
    }
    if (lo < j) quickSortStd(a, lo, j);
    if (i < hi) quickSortStd(a, i, hi);
}

void mergeStd(std::vector<std::string>& a, std::vector<std::string>& aux, int lo, int mid, int hi) {
    std::copy(a.begin()+lo, a.begin()+hi+1, aux.begin()+lo);
    int i = lo, j = mid+1;
    for (int k = lo; k <= hi; ++k) {
        if (i > mid) a[k] = aux[j++];
        else if (j > hi) a[k] = aux[i++];
        else {
            size_t t = 0, n = std::min(aux[i].size(), aux[j].size());
            while (t < n && ++char_comparisons && aux[i][t] == aux[j][t]) ++t;
            ++char_comparisons;
            bool takeLeft = (t < n ? aux[i][t] < aux[j][t] : aux[i].size() < aux[j].size());
            a[k] = takeLeft ? aux[i++] : aux[j++];
        }
    }
}

void mergeSortStd(std::vector<std::string>& a, std::vector<std::string>& aux, int lo, int hi) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    mergeSortStd(a, aux, lo, mid);
    mergeSortStd(a, aux, mid+1, hi);
    mergeStd(a, aux, lo, mid, hi);
}

void quick3Way(std::vector<std::string>& a, int lo, int hi, int d) {
    if (lo >= hi) return;
    int lt = lo, gt = hi;
    char pivot = d < (int)a[lo].size() ? a[lo][d] : '\0';
    int i = lo + 1;
    while (i <= gt) {
        char c = d < (int)a[i].size() ? a[i][d] : '\0';
        ++char_comparisons;
        if (c < pivot) std::swap(a[lt++], a[i++]);
        else if (c > pivot) std::swap(a[i], a[gt--]);
        else ++i;
    }
    quick3Way(a, lo, lt-1, d);
    if (pivot != '\0') quick3Way(a, lt, gt, d+1);
    quick3Way(a, gt+1, hi, d);
}

size_t computeLCP(const std::string &a, const std::string &b) {
    size_t n = std::min(a.size(), b.size()), i = 0;
    while (i < n && ++char_comparisons && a[i] == b[i]) ++i;
    return i;
}

void mergeLCP(std::vector<std::string>& a, std::vector<std::string>& aux, int lo, int mid, int hi) {
    std::copy(a.begin()+lo, a.begin()+hi+1, aux.begin()+lo);
    int i = lo, j = mid+1, k = lo;
    while (i <= mid && j <= hi) {
        size_t lcp = computeLCP(aux[i], aux[j]);
        ++char_comparisons;
        bool takeLeft = (lcp < std::min(aux[i].size(), aux[j].size())
                         ? aux[i][lcp] < aux[j][lcp]
                         : aux[i].size() < aux[j].size());
        a[k++] = takeLeft ? aux[i++] : aux[j++];
    }
    while (i <= mid) a[k++] = aux[i++];
    while (j <= hi) a[k++] = aux[j++];
}

void mergeSortLCP(std::vector<std::string>& a, std::vector<std::string>& aux, int lo, int hi) {
    if (hi - lo + 1 < 16) {
        std::sort(a.begin()+lo, a.begin()+hi+1, CountingComparator());
        return;
    }
    int mid = lo + (hi - lo) / 2;
    mergeSortLCP(a, aux, lo, mid);
    mergeSortLCP(a, aux, mid+1, hi);
    mergeLCP(a, aux, lo, mid, hi);
}

void msdSort(std::vector<std::string>& a, int lo, int hi, int d) {
    if (lo >= hi) return;
    std::vector<int> count(R+2, 0);
    for (int i = lo; i <= hi; ++i) {
        unsigned char c = d < (int)a[i].size() ? a[i][d] : 0;
        count[c+1]++;
    }
    for (int r = 0; r <= R; ++r) count[r+1] += count[r];
    std::vector<std::string> aux(hi-lo+1);
    for (int i = lo; i <= hi; ++i) {
        unsigned char c = d < (int)a[i].size() ? a[i][d] : 0;
        aux[count[c]++] = std::move(a[i]);
    }
    for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i-lo]);
    for (int r = 0; r < R; ++r)
        msdSort(a, lo+count[r], lo+count[r+1]-1, d+1);
}

void msdSortCutoff(std::vector<std::string>& a, int lo, int hi, int d) {
    if (hi - lo + 1 <= MSD_CUTOFF) {
        quick3Way(a, lo, hi, d);
        return;
    }
    std::vector<int> count(R+2, 0);
    for (int i = lo; i <= hi; ++i) {
        unsigned char c = d < (int)a[i].size() ? a[i][d] : 0;
        count[c+1]++;
    }
    for (int r = 0; r <= R; ++r) count[r+1] += count[r];
    std::vector<std::string> aux(hi-lo+1);
    for (int i = lo; i <= hi; ++i) {
        unsigned char c = d < (int)a[i].size() ? a[i][d] : 0;
        aux[count[c]++] = std::move(a[i]);
    }
    for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i-lo]);
    for (int r = 0; r < R; ++r)
        msdSortCutoff(a, lo+count[r], lo+count[r+1]-1, d+1);
}

class StringSortTester {
public:
    void run() {
        std::ofstream out("results.csv");
        out << "N,Case,Algorithm,AvgTime_us,AvgComps\n";
        const int maxN = 3000;
        auto base = gen_.generateRandom(maxN);
        quickSortStd(base, 0, maxN-1);
        std::vector<std::string> aux1(maxN), aux2(maxN);
        mergeSortStd(base, aux1, 0, maxN-1);
        mergeSortLCP(base, aux2, 0, maxN-1);

        for (int n = 100; n <= maxN; n += 100) {
            auto rnd = gen_.generateRandom(n);
            auto rev = gen_.generateReversed(base); rev.resize(n);
            auto almost = gen_.generateAlmostSorted(base, n/20); almost.resize(n);
            runCase(rnd, n, "Random", out);
            runCase(rev, n, "Reversed", out);
            runCase(almost, n, "AlmostSorted", out);
        }
    }

private:
    StringGenerator gen_;
    template<typename F>
    void measure(const std::vector<std::string>& in, F sortF,
                 const std::string& name, int n, const std::string& cs, std::ofstream& out) {
        double timeSum = 0;
        uint64_t compSum = 0;
        for (int t = 0; t < TRIALS; ++t) {
            auto v = in;
            char_comparisons = 0;
            auto t0 = Clock::now();
            sortF(v);
            auto t1 = Clock::now();
            timeSum += Duration(t1 - t0).count();
            compSum += char_comparisons;
        }
        out << n << ',' << cs << ',' << name << ','
            << (timeSum / TRIALS) << ',' << (double(compSum) / TRIALS) << "\n";
    }

    void runCase(const std::vector<std::string>& arr, int n,
                 const std::string& cs, std::ofstream& out) {
        measure(arr, [&](auto& v){ quickSortStd(v,0,n-1); },      "QuickStd",    n, cs, out);
        measure(arr, [&](auto& v){ std::vector<std::string> aux(n); mergeSortStd(v,aux,0,n-1); }, "MergeStd", n, cs, out);
        measure(arr, [&](auto& v){ quick3Way(v,0,n-1,0); },        "Quick3Way",  n, cs, out);
        measure(arr, [&](auto& v){ std::vector<std::string> aux(n); mergeSortLCP(v,aux,0,n-1); }, "MergeLCP", n, cs, out);
        measure(arr, [&](auto& v){ msdSort(v,0,n-1,0); },          "MSDRadix",   n, cs, out);
        measure(arr, [&](auto& v){ msdSortCutoff(v,0,n-1,0); },    "MSDCutoff",  n, cs, out);
    }
};

int main() {
    StringSortTester().run();
    return 0;
}
