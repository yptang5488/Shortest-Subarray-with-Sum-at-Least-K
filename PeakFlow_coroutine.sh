clang++ -std=c++20 PeakFlow_coroutine.cpp -o PeakFlow_coroutine && \
./PeakFlow_coroutine 80 TestTxt/random_10.txt 1 > results/coroutine_sum80_data10_1.txt && \
./PeakFlow_coroutine 15 TestTxt/random_10.txt 1 > results/coroutine_sum15_data10_1.txt && \
./PeakFlow_coroutine 3440 TestTxt/random_80k.txt 10 > results/coroutine_sum3440_data80k_10.txt && \
./PeakFlow_coroutine 800 TestTxt/random_80k.txt 10 > results/coroutine_sum800_data80k_10.txt && \
./PeakFlow_coroutine 2000 TestTxt/random_10m.txt 10 > results/coroutine_sum2000_data10m_10.txt && \
./PeakFlow_coroutine 600 TestTxt/random_10m.txt 10 > results/coroutine_sum600_data10m_10.txt