#include <iostream>
#include <fstream>
#include <vector>
#include <coroutine>
#include <optional>

using namespace std;

vector<long long> read_input(const string &filename) {
    ifstream file(filename);
    vector<long long> sequence;
    int num;
    while (file >> num) {
        sequence.push_back(num);
    }
    return sequence;
}

// define the type of return value of findResultsCoroutine
struct FindResult {
    struct promise_type {
        optional<int> current_value;
        FindResult get_return_object() { 
            return FindResult{coroutine_handle<promise_type>::from_promise(*this)}; 
        }
        suspend_always initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { terminate(); }

        suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
    };


    coroutine_handle<promise_type> handle;
    
    explicit FindResult(coroutine_handle<promise_type> h) : handle(h) {}
    
    // Disable copy construction and copy value
    FindResult(const FindResult&) = delete;
    FindResult& operator=(const FindResult&) = delete;

    // Support for move construction and move value
    FindResult(FindResult&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;  // no longer use
    }

    FindResult& operator=(FindResult&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();  // release resourses
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~FindResult() {
        if (handle) handle.destroy();
    }

    bool next(int &out) {
        if (!handle || handle.done()) return false;
        handle.resume();
        if (handle.promise().current_value) {
            out = *handle.promise().current_value;
            handle.promise().current_value = nullopt; // Clear current_value to prevent duplication
            return true;
        }
        return false;
    }
};

// coroutine : find the shortest subarray
FindResult findResultsCoroutine(const vector<long long>& prefixSum, int len, int target, int start, int end) {
    int n = prefixSum.size();
    for (int i = start; i < end && i + len < n; i++) {
        if (prefixSum[i + len] - prefixSum[i] >= target) {
            co_yield i;
        }
    }
}


// define the type of return value of ifFoundCoroutine
struct FindLength {
    struct promise_type {
        optional<int> current_value;
        static inline atomic<bool> found = false;
        int len;

        FindLength get_return_object() { 
            return FindLength{coroutine_handle<promise_type>::from_promise(*this)}; 
        }
        suspend_always initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { terminate(); }

        suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
    };


    coroutine_handle<promise_type> handle;
    
    explicit FindLength(coroutine_handle<promise_type> h) : handle(h) {}
    
    // Disable copy construction and copy value
    FindLength(const FindLength&) = delete;
    FindLength& operator=(const FindLength&) = delete;

    // Support for move construction and move value
    FindLength(FindLength&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    FindLength& operator=(FindLength&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~FindLength() {
        if (handle) handle.destroy();
    }

    bool next(int &out) {
        if (!handle || handle.done()) return false;
        handle.resume();
        if (handle.promise().current_value) {
            out = *handle.promise().current_value;
            handle.promise().current_value = nullopt;  // Clear current_value to prevent duplication
            return true;
        }
        return false;
    }

    // 動態修改 len 並重新執行
    void set_length(int new_len) {
        if (handle) {
            handle.promise().len = new_len; // 更新長度
        }
    }
};

// coroutine : find the length of shortest subarray
FindLength ifFoundCoroutine(const vector<long long>& prefixSum, int start, int end, int len, int target) {
    co_await std::suspend_never{};
    int n = prefixSum.size();

    while (!FindLength::promise_type::found) {  // 若找到解則停止
        for (int i = start; i < end && i + len < n; i++) {
            if (prefixSum[i + len] - prefixSum[i] >= target) {
                FindLength::promise_type::found = true;  // 設定全局變數通知其他 Coroutine 停止
                co_yield i;  // 回傳找到的索引
                co_return;
            }
        }
        co_yield -1;  // 若未找到則回傳 -1
        co_return;  // 當沒有符合的結果時退出協程！
    }
}


// find the shortest length by binary search method
int findMinLength(const vector<long long>& prefixSum, int numCoroutines, int target) {
    int n = prefixSum.size();
    int len_range_l = 1, len_range_r = n;

    vector<unique_ptr<FindLength>> coroutines;
    coroutines.reserve(numCoroutines);

    // binary search for minimize length
    while (len_range_l < len_range_r) {
        int len = len_range_l + (len_range_r - len_range_l) / 2;

        // 重置 found 狀態
        FindLength::promise_type::found = false;

        // 啟動 coroutines
        coroutines.clear();
        int start = 0;
        for (int i = 0; i < numCoroutines; i++) {
            int tasks_per_coroutine = (n - len + 1) / numCoroutines;
            int extra_tasks = (n - len + 1) % numCoroutines;

            int end = (i < extra_tasks) ? start + tasks_per_coroutine + 1 : start + tasks_per_coroutine;
            coroutines.emplace_back(make_unique<FindLength>(ifFoundCoroutine(prefixSum, start, end, len, target)));
            start = end;
        }

        // 執行 coroutine
        int found_index = -1;
        bool foundThisRound = false;
        for (auto& coroutine : coroutines) {
            int index;
            while (coroutine->next(index)) {
                if (index != -1) {
                    found_index = index;
                    foundThisRound = true;
                    break;
                }
            }
            if (foundThisRound) break;  // if found, stop all searches
        }

        if (found_index == -1) {
            // cout << "No result found for length " << len << endl;
            len_range_l = len + 1;
        } else {
            // cout << "Found at index: " << found_index << " with length " << len << endl;
            len_range_r = len;
        }
    }
    return len_range_l;
}

// find the shorest subarray
void findResults(const vector<long long>& prefixSum, int numCoroutines, int target, int min_length, vector<int>& results) {
    int n = prefixSum.size();
    vector<unique_ptr<FindResult>> coroutines;
    coroutines.reserve(numCoroutines);

    int start = 0;
    for (int i = 0; i < numCoroutines; i++) {
        int tasks_per_coroutine = (n - min_length + 1) / numCoroutines;
        int extra_tasks = (n - min_length + 1) % numCoroutines;

        int end = (i < extra_tasks) ? start + tasks_per_coroutine + 1 : start + tasks_per_coroutine;;
        // cout << start << "," << end << endl;
        coroutines.push_back(make_unique<FindResult>(findResultsCoroutine(prefixSum, min_length, target, start, end)));
        start = end;
    }

    vector<int> indices(numCoroutines);
    vector<bool> hasNext(numCoroutines, true);

    while (true) {
        int Next_cnt = 0;
        for (int i = 0; i < numCoroutines; i++) {
            if (hasNext[i]) {
                hasNext[i] = coroutines[i]->next(indices[i]);
                if (hasNext[i]) {
                    results.push_back(indices[i]);
                    // cout << indices[i] << " ";  // output only if next return true
                    Next_cnt++;
                }
            }
        }
        if(Next_cnt == 0) break;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <target sum> <input file> <numCoroutines>" << endl;
        return 1;
    }

    int target = stoi(argv[1]);
    string filename = argv[2];
    int numCoroutines = stoi(argv[3]);

    // read input
    vector<long long> sequence = read_input(filename);
    int n = sequence.size();

    auto start = chrono::high_resolution_clock::now();
   
    vector<long long> prefixSum(n + 1, 0);

    // build table for prefix sum
    for (int i = 0; i < n; ++i) {
        prefixSum[i + 1] = prefixSum[i] + sequence[i];
    }

    int min_length = findMinLength(prefixSum, numCoroutines, target);
    // cout << "min_length is " << min_length << endl;
    // cout << "--------------------------" << endl;

    vector<int> results;
    findResults(prefixSum, numCoroutines, target, min_length, results);

    // output
    for (int result : results) {
        for (int i = 0; i < min_length; i++) {
            cout << sequence[result + i] << " ";
        }
        cout << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "duration: " << chrono::duration_cast<chrono::nanoseconds>(duration).count() << " ns" << endl;

    return 0;
}
