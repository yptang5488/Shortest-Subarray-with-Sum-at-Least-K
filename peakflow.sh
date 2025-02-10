clang++ -std=c++20 PeakFlow.cpp -o PeakFlow && \
./PeakFlow 80 TestTxt/random_10.txt > results/peakflow_sum80_data10.txt && \
./PeakFlow 15 TestTxt/random_10.txt > results/peakflow_sum15_data10.txt && \
./PeakFlow 3440 TestTxt/random_integers_80k.txt > results/peakflow_sum3440_data80k.txt && \
./PeakFlow 800 TestTxt/random_integers_80k.txt > results/peakflow_sum800_data80k.txt && \
./PeakFlow 2000 TestTxt/random_10m.txt > results/peakflow_sum2000_data10m.txt && \
./PeakFlow 600 TestTxt/random_10m.txt > results/peakflow_sum600_data10m.txt