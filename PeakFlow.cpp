#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>

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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <target sum> <input file>" << endl;
        return 1;
    }

    int target = stoi(argv[1]);
    string filename = argv[2];

    vector<long long> sequence = read_input(filename);
    int n = sequence.size();

    auto start = chrono::high_resolution_clock::now();

    vector<long long> prefixSumVec(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        prefixSumVec[i + 1] = prefixSumVec[i] + sequence[i];
    }

    int min_length = n + 1;
    vector<int> results;
    
    for (int i = 0; i < n; i++) {
        long long targetPrefixSum = prefixSumVec[i] + target;
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
    
    for (int start_idx : results) {
        if (start_idx + min_length > n) continue;
        for (int j = 0; j < min_length; j++) {
            cout << sequence[start_idx + j] << " ";
        }
        cout << endl;
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "duration: " << chrono::duration_cast<chrono::nanoseconds>(duration).count() << " ns" << endl;

    return 0;
}