# Shortest Subarray with Sum at Least K

Peak Flow
Your task is to implement a program in C++ 20 that can efficiently find the shortest continuous subsequences in a given sequence, whose summation is equal to or greater than a specified number. The program must be able to handle non-negative integer inputs and run on the Linux platform with x86-64 architecture.
The input to the program will be a non-negative integer number provided in a text line, and the output should be the subsequences that meet the criteria, displayed in a text line. Additionally, the program should display the total computing time taken to find the subsequences.

```shell
$ cat test.txt
3
17
42
1
26
8
0
63

$ ./peak 80 test.txt
17 42 1 26
26 8 0 63
duration: 1130 ns
```

The program should have a minimum time complexity of O(n) and should be optimized for real-world performance. You are free to use any acceleration methods you choose. Note that the benchmark input data may contain millions of elements.

## solution 0 : Sequential
- 適用於小數量級的資料
- 解題
    1. 計算全局前綴和（prefix sum）`prefixSum`
    2. 利用給定的數列和`target` 在非嚴格遞增的全局前綴和 `prefixSum` 中進行二分搜尋 `O(logn)` 的查找
    3. 輸出全局的最短數列
- 時間複雜度：
    - 時間複雜度為 $O(n\log{n})$

## solution 1 : Multi-thread
- 利用 C++ `std::thread` 進行多執行緒加速計算
- 解題
    1. 計算全局前綴和（prefix sum）`prefixSum`
    2. 將所有子數列可能的起點平均分配給各個執行緒，利用給定的數列和`target` 在非嚴格遞增的全局前綴和進行二分搜尋 `O(logn)` 的查找
    3. 將所有執行緒在各個區間中獲得的局部結果進行合併，輸出全局的最短數列
- 時間複雜度：
    - 假設有 $T$ 個執行緒，時間複雜度為 $O(\frac{n\log{n}}{T})$
- 優化方向
    - 查找使用二分搜尋 `lower_bound` 速度仍較慢
    - 前綴和 `prefixSum` 在多執行序的共用下可能出現資源爭奪的狀況
## solution 2 : Coroutine
- 利用 C++20 的 `Coroutine` 進行協程操作，利用其非同步且輕量化的特性達到加速
- 解題
    1. 計算全局前綴和（prefix sum）`prefixSum`
    2. `findMinLength`：找出最短子數列長度 `min_length`
        - 利用二分搜尋去收斂子數列長度（假設有 `n` 個數，長度範圍為 `[1, n]`）
        - 定義一個協程 `ifFoundCoroutine`，尋找是否存在符合當前子數列長度的數列。若存在，馬上終止所有協程的運作。
        - 將所有可能的起點平均分配給各個協程，利用給定的數列和`target` 在前綴和數列 `prefixSum` 中進行 `O(1)` 的查找
    3. `findResults`：利用最短子數列長度 `min_length` 去尋找子數列
        - 定義一個協程 `findResultsCoroutine`，尋找符合長度的數列
        - 將所有可能的起點平均分配給各個協程，利用給定的數列和`target` 在前綴和數列 `prefixSum` 中進行 `O(1)` 的查找
- 時間複雜度：
    - `findResults` : $O(n\log{n})$
    - `findMinLength` : $O(n)$
- 優化方向
    - 協程設計上，反覆使用 `next()` 去重啟運作，可能會造成龐大開銷
    - 不是使用並行運算，沒有辦法充分利用多核心的特性

## 實驗
程式運作所需時間會隨資料與最短子數列多寡而改變，以下紀錄簡易測試資料的程式用時 : 


|                      Duration                      | 0. Sequential | 1. Multi-thread | 2. Coroutine |
|:--------------------------------------------------:|:-------------:|:---------------:|:------------:|
|    10 data, test 1 <br>(5 results, length = 1)     |    1.6e5*     |      3.5e5      |    1.66e5    |
|    10 data, test 2 <br>(1 results, length = 3)     |    2.86e5     |     4.16e5      |   1.53e5*    |
|  80k data, test 1 <br>(16116 results, length = 1)  |    3.99e7     |     2.99e7      |   2.83e7*    |
|   80k data, test 2 <br>(331 results, length = 4)   |    3.02e7*    |     8.31e6      |    4.04e6    |
| 10m data, test 1 <br>(8008084 results, length = 1) |    7.89e9     |     5.81e9      |   5.45e9*    |
|   10m data, test 2 <br>(12 results, length = 2)    |    2.87e9     |     6.56e8      |   1.48e8*    |

- Sequential 方法：
    - 很明顯的在輸出大量結果時會耗費較多時間
- Multi-thread 方法：
    - 在結果很少時，多執行緒維護的成本會蓋過平行運算帶來的好處
    - 在執行緒數量的設定上還需要更多實驗進行測試
- Coroutine 方法：
    - 雖然沒有做到真正的平行化執行，但是得益於輕量化與低成本 context switch 的特性，不管在結果多或少的測試上，運作時間都有明顯的優勢

更嚴謹的運算成本數據與分析仍須更多實驗以獲得。