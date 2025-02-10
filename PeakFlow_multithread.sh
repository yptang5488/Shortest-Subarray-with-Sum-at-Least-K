clang++ -std=c++20 PeakFlow_multithread.cpp -o PeakFlow_multithread && \
./PeakFlow_multithread 80 TestTxt/random_10.txt 1 > results/multithread_sum80_data10_1.txt && \
./PeakFlow_multithread 15 TestTxt/random_10.txt 1 > results/multithread_sum15_data10_1.txt && \
./PeakFlow_multithread 3440 TestTxt/random_80k.txt 10 > results/multithread_sum3440_data80k_10.txt && \
./PeakFlow_multithread 800 TestTxt/random_80k.txt 10 > results/multithread_sum800_data80k_10.txt && \
./PeakFlow_multithread 2000 TestTxt/random_10m.txt 10 > results/multithread_sum2000_data10m_10.txt && \
./PeakFlow_multithread 600 TestTxt/random_10m.txt 10 > results/multithread_sum600_data10m_10.txt