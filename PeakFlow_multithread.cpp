#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>

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

// prefix[start, found_idx] = sum of seq[start, found_idx-1]
void find_subsequences(const vector<long long>& prefixSumVec,
                        int target,
                        int start,
                        int end,
                        vector<int>& results,
                        int& min_length) {
    long long targetPrefixSum;
    
    for (int i = start; i < end; i++) {
        targetPrefixSum = prefixSumVec[i] + target;

        auto iter = lower_bound(prefixSumVec.begin() + i, prefixSumVec.end(), targetPrefixSum);
        if (iter != prefixSumVec.end()) {
            int found_idx = iter - prefixSumVec.begin();
            if (found_idx > i) {
                int cur_length = found_idx - i;

                if (cur_length < min_length) {
                    min_length = cur_length;
                    results = {i};
                } else if (cur_length == min_length) {
                    results.push_back(i);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <target sum> <input file> <num_threads>" << endl;
        return 1;
    }

    int target = stoi(argv[1]);
    string filename = argv[2];
    int num_threads = stoi(argv[3]);

    // read input
    vector<long long> sequence = read_input(filename);
    int n = sequence.size();

    auto start = chrono::high_resolution_clock::now();

    // build table for prefix sum
    vector<long long> prefixSumVec(n + 1, 0);
    long long prefixSum = 0;

    for (int i = 0; i < n; ++i) {
        prefixSumVec[i + 1] = prefixSumVec[i] + sequence[i];
    }

    vector<thread> threads;
    vector<vector<int>> thread_results(num_threads);
    vector<int> thread_min_lengths(num_threads, n+1);

    // build threads
    int start_idx = 0;
    for (int i = 0; i < num_threads; ++i) {
        int tasks_per_thread = (n + 1) / num_threads;
        int extra_tasks = (n + 1) % num_threads;

        int end_idx = (i < extra_tasks) ? (start_idx + tasks_per_thread + 1) : (start_idx + tasks_per_thread);

        threads.push_back(
            thread(
                find_subsequences,
                cref(prefixSumVec),
                target,
                start_idx,
                end_idx,
                ref(thread_results[i]),
                ref(thread_min_lengths[i])
            )
        );

        start_idx = end_idx;
    }

    for (auto& t : threads) {
        t.join();
    }

    int global_min_length = *min_element(thread_min_lengths.begin(), thread_min_lengths.end());

    for (int i = 0; i < num_threads; i++) {
        if (thread_min_lengths[i] == global_min_length) {
            for (int start_idx : thread_results[i]) {
                if (start_idx + global_min_length > n) continue; // 避免越界
                for (int j = 0; j < global_min_length; j++) {
                    cout << sequence[start_idx + j] << " ";
                }
                cout << endl;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "duration: " << chrono::duration_cast<chrono::nanoseconds>(duration).count() << " ns" << endl;

    return 0;
}